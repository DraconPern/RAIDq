#ifndef _RAID_RS_HPP_
#define _RAID_RS_HPP_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "rs_table.h"
#include "raid_table.h"
#include "gf256.h"
#include "gf256_xmm.h"

//#include "recover.h"

#include "recover.hpp"

//#define _USE_ASM_


template <typename sto_t, unsigned P>
void gen_syndrome(int disks, size_t bytes, void **ptrs, const uint8_t *coeffs[] )
{
	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for( unsigned i=0;i<P;i++) ps[i] = sto_ptrs[z0+1+i];

	sto_t ck_sum[P];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = gfv_mul( sto_ptrs[z0][i] , coeffs[j][z0] );
		for( int z = z0-1; z >= 0 ; z-- ) {
			for(unsigned j=0;j<P;j++) ck_sum[j] ^= gfv_mul( sto_ptrs[z][i] , coeffs[j][z] );
		}
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}


//////////////////////////////////////////////////////////////////////////////


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



#endif
