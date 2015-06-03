#ifndef _ERROR_DECODER_Q_HPP_
#define _ERROR_DECODER_Q_HPP_

#include "gf65536.h"
#include "raidq.hpp"

static inline bool check_1data_err( uint8_t pos , uint8_t s[8] )
{
	uint8_t * ei = s;
	if( s[2] != gf_mul(ei[0],raid6_q[pos])) return false;
	if( s[3] != gf_mul(ei[1],raid6_q[pos])) return false;
	if( s[4] != gf_mul(ei[0],raid7_sqrt[pos])) return false;
	if( s[5] != gf_mul(ei[1],raid7_sqrt[pos])) return false;
	union {
		uint16_t v16;
		uint8_t v8[2];
	}buff;
	buff.v16 = gf216_mul( *(uint16_t*)ei, raidq8_X[pos] );
	if( s[6] != buff.v8[0] ) return false;
	if( s[7] != buff.v8[1] ) return false;
	return true;
}

template <unsigned K>
bool decode_1data_err( uint8_t & position , uint8_t s[8] )
{
	uint8_t v = 0;
	if( s[0] ) {
		v = gf_inv( s[0] );
		v = gf_mul( v , s[2] );
		position = gf_log(v);
		return check_1data_err( position , s );
	}
	if( s[1] ) {
		v = gf_inv( s[1] );
		v = gf_mul( v , s[3] );
		position = gf_log(v);
		return check_1data_err( position , s );
	}
	return false;
}


/////////////////////////////////////////////////////

/// L(X) = (1+a^-i/2 X)(1+a^-j/2 X) = 1 + l1 X + l2 X^2
/// e0 [ L(a^i/2) = 0 = 1 + l1 a^i/2 + l2 a^i ]
/// e1 [ L(a^j/2) = 0 = 1 + l1 a^j/2 + l2 a^j ]
/// S0 + l1 S2 + l2 S1 = 0

///  [ S20  , S10 ]  [ l1 ] = [ S00 ]
///  [ S21  , S11 ]  [ l2 ]   [ S01 ]

static inline bool solve_ELP( uint8_t elp[2] , uint8_t s[8] )
{
	uint8_t s20 = s[4];
	uint8_t s21 = s[5];
	uint8_t s10 = s[2];
	uint8_t s11 = s[3];
	elp[0] = s[0];
	elp[1] = s[1];

	uint8_t tmp;
	if( 0 == s20 ) {
		if( 0 == s21 ) return false;
		tmp = s20; s20 = s21; s21 = 0;
		tmp = elp[0]; elp[0] = elp[1]; elp[1]=tmp;
	}

	uint8_t inv_s20 = gf_inv( s20 );  /// s20 = 1;
	s10 = gf_mul( inv_s20 , s10 );
	elp[0] = gf_mul( inv_s20 , elp[0] );

	s11 ^= gf_mul( s10 , s21 );
	elp[1] ^= gf_mul( elp[0] , s21 );

	if( 0 == s11 ) return false;
	uint8_t inv_s11 = gf_inv( s11 ); /// s11 = 1;
	elp[1] = gf_mul( elp[1] , inv_s11 );
	elp[0] ^= gf_mul( s10 , elp[1] );
	return true;
}

static inline uint8_t eva_ELP( uint8_t elp[2] , uint8_t i )
{
	return 1 ^ gf_mul( elp[0] , raid7_sqrt[i] ) ^ gf_mul( elp[1] , raid6_q[i] );
}


///////////////////////////////////////////////

/// [ 1   1   ] [ei] = S0
/// [ a^i a^j ] [ej] = S1
static inline void solve_ei_ej( uint8_t ei[2] , uint8_t ej[2] , uint8_t i , uint8_t j , uint8_t s[8] )
{
	ei[0] = s[0];
	ei[1] = s[1];
	ej[0] = s[2];
	ej[1] = s[3];

	ej[0] ^= gf_mul( ei[0] , raid6_q[i] );
	ej[1] ^= gf_mul( ei[1] , raid6_q[i] );
	uint8_t inv_aipaj = gf_inv( raid6_q[i]^raid6_q[j] );
	ej[0] = gf_mul( ej[0] , inv_aipaj );
	ej[1] = gf_mul( ej[1] , inv_aipaj );
	ei[0] ^= ej[0];
	ei[1] ^= ej[1];
}


template <unsigned K>
bool decode_2data_err_ELP( uint8_t ei[2] , uint8_t ej[2] , uint8_t & pi , uint8_t & pj , uint8_t s[8] )
{
	uint8_t elp[2];
	if( ! solve_ELP( elp , s ) ) return false;
	unsigned flag = 0;
	for( unsigned i=0;i<K;i++) {
		if( 0 == eva_ELP( elp , i ) ){
			if( 0 == flag ) {
				pi = i;
				flag++;
				continue;
			}
			if( 1 == flag ) {
				pj = i;
				break;
			}
		}
	}
	solve_ei_ej( ei , ej , pi , pj , s );
	return true;
}


/////////////////////////////////////////////////

static inline bool check_sqrt_X( uint8_t ei[2] , uint8_t ej[2] , uint8_t i , uint8_t j , uint8_t s[8] )
{
	if( s[4] != (gf_mul(ei[0],raid7_sqrt[i])^gf_mul(ej[0],raid7_sqrt[j])) ) return false;
	if( s[5] != (gf_mul(ei[1],raid7_sqrt[i])^gf_mul(ej[1],raid7_sqrt[j])) ) return false;

	union{
	uint16_t v16;
	uint8_t v8[2];
	} buff;
	buff.v16 = gf216_mul( raidq8_X[i] , *(uint16_t*)ei )^gf216_mul( raidq8_X[j] , *(uint16_t*)ej );
	if( s[6] != buff.v8[0] ) return false;
	if( s[7] != buff.v8[1] ) return false;
	return true;
}

template <unsigned K>
bool decode_2data_err_enum( uint8_t ei[2] , uint8_t ej[2] , uint8_t & pi , uint8_t & pj , uint8_t s[8] )
{
	for( unsigned i=0;i<K;i++) {
		for(unsigned j=i+1;j<K;j++) {
			solve_ei_ej( ei , ej , i , j , s );
			if( check_sqrt_X( ei , ej , i , j , s ) ) {
				pi = i;
				pj = j;
				return true;
			}
		}
	}
	return false;
}

/////////////////////////////////////

template <unsigned K>
bool raidq8_X_decode_error( uint8_t ei[2] , uint8_t ej[2] , uint8_t & i , uint8_t & j , uint8_t s[8] )
{
	/// XXX: # ( s[0] == 0 ) <= 2 , errors in check-sums

	if( decode_1data_err<K>( i , s ) ) {
		ei[0] = s[0];
		ei[1] = s[1];
		return true;
	}

	if( decode_2data_err_ELP<K>(ei,ej,i,j,s) ) return true;

	if( decode_2data_err_enum<K>(ei,ej,i,j,s) ) return true;

	return false;
}

//////////////////////////////////////////

#include "gf256_xmm.h"

template <unsigned K>
unsigned raidq8_X_error_decoder_ymmx2( size_t bytes , void ** void_ptrs )
{
	const unsigned P = 4;

	ymm_x2 ** s_ptrs = (ymm_x2**) void_ptrs;

	ymm_x2 syns[P];

	union {
		__m256i v256;
		uint8_t v8[32];
	} buff;

	unsigned times = bytes/sizeof(ymm_x2);

	uint32_t s0,s1,s2,s3;

	unsigned fails = 0;
	unsigned corrs = 0;
	uint8_t e0[2];
	uint8_t e1[2];
	uint8_t s[8];
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) syns[j] = s_ptrs[j][i];
		s0 = (~_mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_setzero_si256(),syns[0].ymm0) ))
			| (~_mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_setzero_si256(),syns[0].ymm1) ));
		s1 = (~_mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_setzero_si256(),syns[1].ymm0) ))
			| (~_mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_setzero_si256(),syns[1].ymm1) ));
		s2 = (~_mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_setzero_si256(),syns[2].ymm0) ))
			| (~_mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_setzero_si256(),syns[2].ymm1) ));
		s3 = (~_mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_setzero_si256(),syns[3].ymm0) ))
			| (~_mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_setzero_si256(),syns[3].ymm1) ));

		for(unsigned k=0;k<32;k++) {
			if( (s0&1) || (s1&1) || (s2&1) || (s3&1) ) {
				_mm256_store_si256( &buff.v256 , syns[0].ymm0 );
				s[0] = buff.v8[k];
				_mm256_store_si256( &buff.v256 , syns[0].ymm1 );
				s[1] = buff.v8[k];
				_mm256_store_si256( &buff.v256 , syns[1].ymm0 );
				s[2] = buff.v8[k];
				_mm256_store_si256( &buff.v256 , syns[1].ymm1 );
				s[3] = buff.v8[k];
				_mm256_store_si256( &buff.v256 , syns[2].ymm0 );
				s[4] = buff.v8[k];
				_mm256_store_si256( &buff.v256 , syns[2].ymm1 );
				s[5] = buff.v8[k];
				_mm256_store_si256( &buff.v256 , syns[3].ymm0 );
				s[6] = buff.v8[k];
				_mm256_store_si256( &buff.v256 , syns[3].ymm1 );
				s[7] = buff.v8[k];

				uint8_t pi,pj;
				if( raidq8_X_decode_error<K>( e0 , e1 , pi, pj, s ) ) {
					/// correct errors
					corrs ++;
				} else 	fails++;
			}
			s0 >>= 1;
			s1 >>= 1;
			s2 >>= 1;
			s3 >>= 1;
		}
	}

	return fails + corrs;
}




template <typename sto_t>
void raidq8_X_diff_syndrome( int datas, size_t bytes, void ** void_ptrs )
{
	assert(0==(1&datas));
	const unsigned P = 4;

	//get_rec_coef4_xor_gf216( rec_coef, fail_idx , raid_sqrt , raidq8_X );

	sto_t ** sto_ptrs = (sto_t**) void_ptrs;

	sto_t *ps[P];
	sto_t *syns[P];
	for( unsigned i=0;i<P;i++) {
		ps[i] = sto_ptrs[datas+i];
		syns[i] = sto_ptrs[datas+P+i];
	}

	int z0 = datas - 1;
	sto_t ck_sum[P+1];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		mul_alpha(ck_sum[1]);
		gf216bsv_mul_0x0100(ck_sum[3]);
		ck_sum[0]^= sto_ptrs[z0-1][i];
		ck_sum[1]^= sto_ptrs[z0-1][i];
		ck_sum[4] = sto_ptrs[z0-1][i];
		ck_sum[3]^= sto_ptrs[z0-1][i];

		for( int z = z0-2; z >= 0 ; z-=2 ) {
			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[2]);
			gf216bsv_mul_0x0100(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z][i];
			ck_sum[1]^=sto_ptrs[z][i];
			ck_sum[2]^=sto_ptrs[z][i];
			ck_sum[3]^=sto_ptrs[z][i];

			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[4]);
			gf216bsv_mul_0x0100(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z-1][i];
			ck_sum[1]^=sto_ptrs[z-1][i];
			ck_sum[3]^=sto_ptrs[z-1][i];
			ck_sum[4]^=sto_ptrs[z-1][i];
		}
		mul_sqrt_alpha(ck_sum[2]);
		ck_sum[2]^=ck_sum[4];
		for(unsigned j=0;j<P;j++) syns[j][i] = ps[j][i] ^ ck_sum[j];
	}
}



//////////////////////////////////////////////////////////////////////////////


/*
/// CAUTION: fail_idx[i] < datas  and ascending ordering.
template <typename sto_t,unsigned fail_datas>
void raid_rs_recov_data( int datas, size_t bytes, uint8_t fail_idx[], void ** void_ptrs , const uint8_t *coeffs[] )
{
	const unsigned fails = fail_datas;
	const unsigned P = fail_datas;

	uint8_t coe0[4];
	uint8_t coe1[4];
	uint8_t coe2[4];
	uint8_t coe3[4];
	uint8_t * rec_coef[] = { coe0, coe1, coe2, coe3 };
	get_rec_coef<fail_datas>( rec_coef, fail_idx , coeffs );

	sto_t ** sto_ptrs = (sto_t**) void_ptrs;
	sto_t * fail_ptrs[fails];
	for(unsigned i=0;i<fails;i++) {
		fail_ptrs[i] = sto_ptrs[fail_idx[i]];
		sto_ptrs[fail_idx[i]] = (sto_t*)raid6_empty_zero_page;
	}

	sto_t *ps[P];
	for( unsigned i=0;i<P;i++) ps[i] = sto_ptrs[datas+i];

	int z0 = datas - 1;
	sto_t ck_sum[P];
	sto_t res;
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = ps[j][i];
		for( int z = z0; z >= 0 ; z-- ) {
			for(unsigned j=0;j<P;j++) ck_sum[j] ^= gfv_mul( sto_ptrs[z][i] , coeffs[j][z] );
		}
		for(unsigned j=0;j<P;j++) {
			res = gfv_mul( ck_sum[0] , rec_coef[j][0] );
			for( unsigned k=1;k<P;k++) res ^= gfv_mul( ck_sum[k] , rec_coef[j][k] );
			fail_ptrs[j][i] = res;
		}
	}

	for(unsigned i=0;i<fails;i++) {
		sto_ptrs[fail_idx[i]] = fail_ptrs[i];
	}
}
*/


#endif
