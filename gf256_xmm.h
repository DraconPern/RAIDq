#ifndef _GF256_XMM_H_
#define _GF256_XMM_H_


#include "gf256.h"

//#define _NO_AVX2_

#define USE_BIT_SHIFT

// #define USE_SSE2_MUL_A

// GF(256)
// primitive polynomail x^8 + x^4 + x^3 + x^2 + 1
// 0x11d



inline uint64_t gen_mask( uint64_t msb ) {
	uint64_t ret = msb;
	ret |= (ret>>1);
	ret |= (ret>>2);
	ret |= (ret>>4);
	return ret;
}


inline uint64_t gfv_mul( uint64_t a , uint8_t b ) {
#if 0 /// slow
	uint64_t ret = 0;
	const uint64_t msb = 0x8080808080808080ull;
	const uint64_t gp =  0x1d1d1d1d1d1d1d1dull;
	uint8_t check_bit = 0x01;

	if(b&check_bit) ret = a;  // * 1
	for( unsigned i=0;i<7;i++) {
		/// mul 2
		uint64_t msk = gen_mask( a & msb );
		a &= (~msb);
		a = (a<<1) ^ (msk&gp);
		check_bit <<= 1;
		if( b & check_bit ) ret ^= a;
	}
	return ret;
#else /// faster
	if( 0 == b ) return 0;
	if( 1 == b ) return a;
	uint16_t log_b = gf_log( b );
	uint8_t * aa = (uint8_t *)(&a);
	union{
	uint64_t ret;
	uint8_t rr[8];
	};
	for(unsigned i=0;i<8;i++) rr[i] = (0==aa[i])?0:gf_alog(log_b+gf_log(aa[i]));
	return ret;
#endif
}


// alpha 0x02
inline void mul_alpha( uint64_t & a ) {
	a = gfv_mul( a , 0x02 );
}

inline void mul_sqrt_alpha( uint64_t & a ) {
	a = gfv_mul( a , 0x85 );
}

// alpha2 0x04
inline void mul_alpha2( uint64_t & a ) {
	a = gfv_mul( a , 0x04 );
}

// alpha3 0x08
inline void mul_alpha3( uint64_t & a ) {
	a = gfv_mul( a , 0x08 );
}

// alpha4 0x10
inline void mul_alpha4( uint64_t & a ) {
	a = gfv_mul( a , 0x10 );
}




#ifndef _NO_SSE_


#include <tmmintrin.h>


//#define TAB2

inline __m128i gfv_mul( const __m128i a , uint8_t b ) {
	__m128i mask_low = *(__m128i*)_mask_low;
//	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
#ifdef TAB2
	__m128i & tab_l = ((__m128i *)(&sse_mul_tab_l[0]))[b];
	__m128i & tab_h = ((__m128i *)(&sse_mul_tab_h[0]))[b];
//	__m128i tab_l = _mm_load_si128( &(((__m128i *)sse_mul_tab_l)[b]));
//	__m128i tab_h = _mm_load_si128( &(((__m128i *)sse_mul_tab_h)[b]));
#else
        __m128i & tab_h = ((__m128i*)sse_mul_tab)[((uint16_t)b)<<1];
        __m128i & tab_l = ((__m128i*)sse_mul_tab)[(((uint16_t)b)<<1)+1];
#endif
	return _mm_shuffle_epi8( tab_l, a&mask_low )^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a,4)&mask_low );
}


inline void and_not( __m128i & a , __m128i b )
{
	a = _mm_andnot_si128( a, b );
}

inline void mul_sqrt_alpha( __m128i & a ) {
	__m128i mask_low = *(__m128i*)_mask_low;
#ifdef TAB2
//	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i & tab_l = ((__m128i *)(&sse_mul_tab_l[0]))[SQRT_ALPHA];
	__m128i & tab_h = ((__m128i *)(&sse_mul_tab_h[0]))[SQRT_ALPHA];
#else
        __m128i & tab_h = ((__m128i*)sse_mul_tab)[SQRT_ALPHA*2];
        __m128i & tab_l = ((__m128i*)sse_mul_tab)[SQRT_ALPHA*2+1];
#endif
	a = _mm_shuffle_epi8( tab_l, a&mask_low )^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a,4)&mask_low );
}





// alpha 0x02
inline void mul_alpha_sse2( __m128i & a ) {
	__m128i ppoly = _mm_load_si128((__m128i *)_pri_poly);
	__m128i zero = _mm_setzero_si128();
	a = _mm_add_epi8( a , a ) ^ _mm_and_si128( _mm_cmplt_epi8(a,zero) , ppoly );
}

#ifdef USE_SSE2_MUL_A
inline void mul_alpha( __m128i & a ) {
	__m128i ppoly = _mm_load_si128((__m128i *)_pri_poly);
	__m128i zero = _mm_setzero_si128();
	a = _mm_add_epi8( a , a ) ^ _mm_and_si128( _mm_cmplt_epi8(a,zero) , ppoly );
}
#else
/// ssse3
inline void mul_alpha( __m128i & a ) {
	__m128i ppoly = _mm_load_si128((__m128i *)_pri_poly);
//	__m128i &ppoly = *(__m128i*)_pri_poly;
	a = _mm_add_epi8( a , a ) ^ ppoly ^ _mm_shuffle_epi8( ppoly , a );
}
#endif

inline void mul_alpha_brvs( __m128i & a ) {
	__m128i ppoly = _mm_load_si128((__m128i *)_brvs_irr_poly);
	a = _mm_add_epi8( a , a ) ^ _mm_shuffle_epi8( ppoly , a );
}

inline void bit_reverse( __m128i & a ) {
	__m128i brvs = _mm_load_si128((__m128i *)_mask_brvs);
	a ^= brvs;
}



// alpha2 0x04
inline void mul_alpha2( __m128i & a ) {
#if 0
	__m128i brvs7bit = _mm_load_si128((__m128i *)_brvs_irr_poly);
	__m128i brvs8bit = _mm_load_si128((__m128i *)_brvs_9thbit);
	__m128i t = _mm_add_epi8( a , a );
	a = _mm_add_epi8( t , t ) ^ _mm_shuffle_epi8( brvs7bit , t ) ^_mm_shuffle_epi8( brvs8bit , a );
#endif
#if 0
	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i & tab_l = ((__m128i *)(&sse_mul_tab_l[0]))[4*16];
	__m128i & tab_h = ((__m128i *)(&sse_mul_tab_h[0]))[4*16];
	a = _mm_shuffle_epi8( tab_l, a&mask_low )^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a,4)&mask_low );
#endif
//	__m128i & tab_h = * ((__m128i *)(&sse_mul_tab_h[4*16]));
	__m128i & tab_h = * ((__m128i *)(&sse_mul_tab[4*32]));
//	__m128i & tab_h = ((__m128i *)sse_mul_tab)[4*2];
#ifdef USE_BIT_SHIFT
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
	a = _mm_slli_epi16(a&mask_low,2)^_mm_shuffle_epi8( tab_h , _mm_srli_epi16(a,4)&mask_low );
#else
//	__m128i mask_67bit = _mm_load_si128( (__m128i*) _mask_67bit );
	__m128i & mask_67bit = *(__m128i*) _mask_67bit;
	__m128i t = _mm_add_epi8(a,a);
	a = _mm_add_epi8(t,t)^_mm_shuffle_epi8( tab_h , _mm_srli_epi16(a&mask_67bit,4) );
#endif
}

// alpha3 0x08
inline void mul_alpha3( __m128i & a ) {
	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i & tab_h = * ((__m128i *)(&sse_mul_tab_h[8*16]));
	a = _mm_slli_epi32(a&mask_low,3)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a,4)&mask_low );
}

// alpha4 0x10
inline void mul_alpha4( __m128i & a ) {
	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i & tab_h = * ((__m128i *)(&sse_mul_tab_h[16*16]));
	a = _mm_slli_epi32(a&mask_low,4)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a,4)&mask_low );
}



//////////////////////////////////////////////////////

struct xmm_x2 {
	__m128i xmm0;
	__m128i xmm1;
};

inline const xmm_x2 operator^( const xmm_x2 & a , const xmm_x2 & b ) {
	xmm_x2 ret;
	ret.xmm0 = a.xmm0^b.xmm0;
	ret.xmm1 = a.xmm1^b.xmm1;
	return ret;
}

inline xmm_x2 & operator^=( xmm_x2 & a , const xmm_x2 & b) {
	a.xmm0^=b.xmm0;
	a.xmm1^=b.xmm1;
	return a;
}

inline xmm_x2 & operator&=( xmm_x2 & a , const xmm_x2 & b) {
	a.xmm0&=b.xmm0;
	a.xmm1&=b.xmm1;
	return a;
}

inline xmm_x2 & operator|=( xmm_x2 & a , const xmm_x2 & b) {
	a.xmm0|=b.xmm0;
	a.xmm1|=b.xmm1;
	return a;
}

inline void and_not( xmm_x2 & a , const xmm_x2 & b ) {
	and_not( a.xmm0 , b.xmm0 );
	and_not( a.xmm1 , b.xmm1 );
}

inline const xmm_x2 gfv_mul( const xmm_x2 & a , uint8_t b ) {
	__m128i mask_low = *(__m128i*)_mask_low;
#ifdef TAB2
//	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i & tab_l = ((__m128i *)(&sse_mul_tab_l[0]))[b];
	__m128i & tab_h = ((__m128i *)(&sse_mul_tab_h[0]))[b];
//	__m128i tab_l = _mm_load_si128( &(((__m128i *)sse_mul_tab_l)[b]));
//	__m128i tab_h = _mm_load_si128( &(((__m128i *)sse_mul_tab_h)[b]));
#else
        __m128i & tab_h = ((__m128i*)sse_mul_tab)[((uint16_t)b)<<1];
        __m128i & tab_l = ((__m128i*)sse_mul_tab)[(((uint16_t)b)<<1)+1];
#endif
	xmm_x2 ret;
	ret.xmm0 = _mm_shuffle_epi8(tab_l,a.xmm0&mask_low)^_mm_shuffle_epi8(tab_h,_mm_srli_epi32(a.xmm0,4)&mask_low);
	ret.xmm1 = _mm_shuffle_epi8(tab_l,a.xmm1&mask_low)^_mm_shuffle_epi8(tab_h,_mm_srli_epi32(a.xmm1,4)&mask_low);
	return ret;
}


inline void mul_sqrt_alpha( xmm_x2 & a ) {
	mul_sqrt_alpha( a.xmm0 );
	mul_sqrt_alpha( a.xmm1 );
}



// alpha 0x02
inline void mul_alpha_sse2( xmm_x2 & a ) {
	__m128i zero = _mm_setzero_si128();
	__m128i ppoly = _mm_load_si128((__m128i *)_pri_poly);
	a.xmm0 = _mm_add_epi8( a.xmm0 , a.xmm0 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm0,zero) , ppoly );
	a.xmm1 = _mm_add_epi8( a.xmm1 , a.xmm1 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm1,zero) , ppoly );
}

#ifdef USE_SSE2_MUL_A
inline void mul_alpha( xmm_x2 & a ) {
	__m128i zero = _mm_setzero_si128();
	__m128i ppoly = _mm_load_si128((__m128i *)_pri_poly);
	a.xmm0 = _mm_add_epi8( a.xmm0 , a.xmm0 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm0,zero) , ppoly );
	a.xmm1 = _mm_add_epi8( a.xmm1 , a.xmm1 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm1,zero) , ppoly );
}
#else
inline void mul_alpha( xmm_x2 & a ) {
//	__m128i & mask_msb = * ((__m128i *)(&_msb_epi8[0]));
	__m128i ppoly = _mm_load_si128((__m128i *)_pri_poly);
	a.xmm0 = _mm_add_epi8( a.xmm0 , a.xmm0 ) ^ ppoly ^ _mm_shuffle_epi8( ppoly , a.xmm0 );
	a.xmm1 = _mm_add_epi8( a.xmm1 , a.xmm1 ) ^ ppoly ^ _mm_shuffle_epi8( ppoly , a.xmm1 );
}
#endif

inline void mul_alpha_brvs( xmm_x2 & a ) {
	__m128i ppoly = _mm_load_si128((__m128i *)_brvs_irr_poly);
	a.xmm0 = _mm_add_epi8( a.xmm0 , a.xmm0 ) ^ _mm_shuffle_epi8( ppoly , a.xmm0 );
	a.xmm1 = _mm_add_epi8( a.xmm1 , a.xmm1 ) ^ _mm_shuffle_epi8( ppoly , a.xmm1 );
}

inline void bit_reverse( xmm_x2 & a ) {
	__m128i brvs = _mm_load_si128((__m128i *)_mask_brvs);
	a.xmm0 ^= brvs;
	a.xmm1 ^= brvs;
}

// alpha2 0x04
inline void mul_alpha2( xmm_x2 & a ) {
#if 1
	mul_alpha2( a.xmm0 );
	mul_alpha2( a.xmm1 );
#else
	__m128i & tab_h = * ((__m128i *)(&sse_mul_tab_h[4*16]));
#ifdef USE_BIT_SHIFT
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
	a.xmm0 = _mm_slli_epi16(a.xmm0&mask_low,2)^_mm_shuffle_epi8( tab_h , _mm_srli_epi16(a.xmm0,4)&mask_low );
	a.xmm1 = _mm_slli_epi16(a.xmm1&mask_low,2)^_mm_shuffle_epi8( tab_h , _mm_srli_epi16(a.xmm1,4)&mask_low );
#else
	__m128i mask_67bit = _mm_load_si128( (__m128i*) _mask_67bit );
	__m128i t0 = _mm_add_epi8(a.xmm0,a.xmm0);
	__m128i t1 = _mm_add_epi8(a.xmm1,a.xmm1);
	a.xmm0 = _mm_add_epi8(t0,t0)^_mm_shuffle_epi8( tab_h , _mm_srli_epi16(a.xmm0&mask_67bit,4) );
	a.xmm1 = _mm_add_epi8(t1,t1)^_mm_shuffle_epi8( tab_h , _mm_srli_epi16(a.xmm1&mask_67bit,4) );
#endif
#endif
}

// alpha3 0x08
inline void mul_alpha3( xmm_x2 & a ) {
	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i & tab_h = * ((__m128i *)(&sse_mul_tab_h[8*16]));
	a.xmm0 = _mm_slli_epi32(a.xmm0&mask_low,3)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm0,4)&mask_low );
	a.xmm1 = _mm_slli_epi32(a.xmm1&mask_low,3)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm1,4)&mask_low );
}

// alpha4 0x10
inline void mul_alpha4( xmm_x2 & a ) {
	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i & tab_h = * ((__m128i *)(&sse_mul_tab_h[16*16]));
	a.xmm0 = _mm_slli_epi32(a.xmm0&mask_low,4)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm0,4)&mask_low );
	a.xmm1 = _mm_slli_epi32(a.xmm1&mask_low,4)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm1,4)&mask_low );
}

//////////////////////////////////////////////////////////////////




struct xmm_x4 {
	__m128i xmm0;
	__m128i xmm1;
	__m128i xmm2;
	__m128i xmm3;
};

inline const xmm_x4 operator^( const xmm_x4 & a , const xmm_x4 & b ) {
	xmm_x4 ret;
	ret.xmm0 = a.xmm0^b.xmm0;
	ret.xmm1 = a.xmm1^b.xmm1;
	ret.xmm2 = a.xmm2^b.xmm2;
	ret.xmm3 = a.xmm3^b.xmm3;
	return ret;
}

inline xmm_x4 & operator^=( xmm_x4 & a , const xmm_x4 & b) {
	a.xmm0^=b.xmm0;
	a.xmm1^=b.xmm1;
	a.xmm2^=b.xmm2;
	a.xmm3^=b.xmm3;
	return a;
}

inline xmm_x4 & operator&=( xmm_x4 & a , const xmm_x4 & b) {
	a.xmm0&=b.xmm0;
	a.xmm1&=b.xmm1;
	a.xmm2&=b.xmm2;
	a.xmm3&=b.xmm3;
	return a;
}

inline xmm_x4 & operator|=( xmm_x4 & a , const xmm_x4 & b) {
	a.xmm0|=b.xmm0;
	a.xmm1|=b.xmm1;
	a.xmm2|=b.xmm2;
	a.xmm3|=b.xmm3;
	return a;
}

inline void and_not( xmm_x4 & a , const xmm_x4 & b ) {
	and_not( a.xmm0 , b.xmm0 );
	and_not( a.xmm1 , b.xmm1 );
	and_not( a.xmm2 , b.xmm2 );
	and_not( a.xmm3 , b.xmm3 );
}

inline const xmm_x4 gfv_mul( const xmm_x4 & a , uint8_t b ) {
	__m128i mask_low = *(__m128i*)_mask_low;
#ifdef TAB2
//	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i & tab_l = ((__m128i *)(&sse_mul_tab_l[0]))[b];
	__m128i & tab_h = ((__m128i *)(&sse_mul_tab_h[0]))[b];
//	__m128i tab_l = _mm_load_si128( &(((__m128i *)sse_mul_tab_l)[b]));
//	__m128i tab_h = _mm_load_si128( &(((__m128i *)sse_mul_tab_h)[b]));
#else
        __m128i & tab_h = ((__m128i*)sse_mul_tab)[((uint16_t)b)<<1];
        __m128i & tab_l = ((__m128i*)sse_mul_tab)[(((uint16_t)b)<<1)+1];
#endif
	xmm_x4 ret;
	ret.xmm0 = _mm_shuffle_epi8(tab_l,a.xmm0&mask_low)^_mm_shuffle_epi8(tab_h,_mm_srli_epi32(a.xmm0,4)&mask_low);
	ret.xmm1 = _mm_shuffle_epi8(tab_l,a.xmm1&mask_low)^_mm_shuffle_epi8(tab_h,_mm_srli_epi32(a.xmm1,4)&mask_low);
	ret.xmm2 = _mm_shuffle_epi8(tab_l,a.xmm2&mask_low)^_mm_shuffle_epi8(tab_h,_mm_srli_epi32(a.xmm2,4)&mask_low);
	ret.xmm3 = _mm_shuffle_epi8(tab_l,a.xmm3&mask_low)^_mm_shuffle_epi8(tab_h,_mm_srli_epi32(a.xmm3,4)&mask_low);
	return ret;
}



inline void mul_sqrt_alpha( xmm_x4 & a ) {
	mul_sqrt_alpha( a.xmm0 );
	mul_sqrt_alpha( a.xmm1 );
	mul_sqrt_alpha( a.xmm2 );
	mul_sqrt_alpha( a.xmm3 );
}




// alpha 0x02
inline void mul_alpha_sse2( xmm_x4 & a ) {
	__m128i zero = _mm_setzero_si128();
	__m128i ppoly = _mm_load_si128((__m128i *)_pri_poly);
	a.xmm0 = _mm_add_epi8( a.xmm0 , a.xmm0 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm0,zero) , ppoly );
	a.xmm1 = _mm_add_epi8( a.xmm1 , a.xmm1 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm1,zero) , ppoly );
	a.xmm2 = _mm_add_epi8( a.xmm2 , a.xmm2 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm2,zero) , ppoly );
	a.xmm3 = _mm_add_epi8( a.xmm3 , a.xmm3 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm3,zero) , ppoly );
}

#ifdef USE_SSE2_MUL_A
inline void mul_alpha( xmm_x4 & a ) {
	__m128i zero = _mm_setzero_si128();
	__m128i ppoly = _mm_load_si128((__m128i *)_pri_poly);
	a.xmm0 = _mm_add_epi8( a.xmm0 , a.xmm0 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm0,zero) , ppoly );
	a.xmm1 = _mm_add_epi8( a.xmm1 , a.xmm1 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm1,zero) , ppoly );
	a.xmm2 = _mm_add_epi8( a.xmm2 , a.xmm2 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm2,zero) , ppoly );
	a.xmm3 = _mm_add_epi8( a.xmm3 , a.xmm3 ) ^ _mm_and_si128( _mm_cmplt_epi8(a.xmm3,zero) , ppoly );
}
#else
inline void mul_alpha( xmm_x4 & a ) {
//	__m128i & mask_msb = * ((__m128i *)(&_msb_epi8[0]));
	__m128i ppoly = _mm_load_si128((__m128i *)_pri_poly);
	a.xmm0 = _mm_add_epi8( a.xmm0 , a.xmm0 ) ^ ppoly ^ _mm_shuffle_epi8( ppoly , a.xmm0 );
	a.xmm1 = _mm_add_epi8( a.xmm1 , a.xmm1 ) ^ ppoly ^ _mm_shuffle_epi8( ppoly , a.xmm1 );
	a.xmm2 = _mm_add_epi8( a.xmm2 , a.xmm2 ) ^ ppoly ^ _mm_shuffle_epi8( ppoly , a.xmm2 );
	a.xmm3 = _mm_add_epi8( a.xmm3 , a.xmm3 ) ^ ppoly ^ _mm_shuffle_epi8( ppoly , a.xmm3 );

}
#endif

inline void mul_alpha_brvs( xmm_x4 & a ) {
	__m128i ppoly = _mm_load_si128((__m128i *)_brvs_irr_poly);
	a.xmm0 = _mm_add_epi8( a.xmm0 , a.xmm0 ) ^ _mm_shuffle_epi8( ppoly , a.xmm0 );
	a.xmm1 = _mm_add_epi8( a.xmm1 , a.xmm1 ) ^ _mm_shuffle_epi8( ppoly , a.xmm1 );
	a.xmm2 = _mm_add_epi8( a.xmm2 , a.xmm2 ) ^ _mm_shuffle_epi8( ppoly , a.xmm2 );
	a.xmm3 = _mm_add_epi8( a.xmm3 , a.xmm3 ) ^ _mm_shuffle_epi8( ppoly , a.xmm3 );
}

inline void bit_reverse( xmm_x4 & a ) {
	__m128i brvs = _mm_load_si128((__m128i *)_mask_brvs);
	a.xmm0 ^= brvs;
	a.xmm1 ^= brvs;
	a.xmm2 ^= brvs;
	a.xmm3 ^= brvs;
}


// alpha2 0x04
inline void mul_alpha2( xmm_x4 & a ) {
#if 1
	mul_alpha2( a.xmm0 );
	mul_alpha2( a.xmm1 );
	mul_alpha2( a.xmm2 );
	mul_alpha2( a.xmm3 );
#else
	__m128i & tab_h = * ((__m128i *)(&sse_mul_tab_h[4*16]));
#ifdef USE_BIT_SHIFT
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
	a.xmm0 = _mm_slli_epi32(a.xmm0&mask_low,2)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm0,4)&mask_low );
	a.xmm1 = _mm_slli_epi32(a.xmm1&mask_low,2)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm1,4)&mask_low );
	a.xmm2 = _mm_slli_epi32(a.xmm2&mask_low,2)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm2,4)&mask_low );
	a.xmm3 = _mm_slli_epi32(a.xmm3&mask_low,2)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm3,4)&mask_low );
#else
	__m128i mask_67bit = _mm_load_si128( (__m128i*) _mask_67bit );
	__m128i t0 = _mm_add_epi8(a.xmm0,a.xmm0);
	__m128i t1 = _mm_add_epi8(a.xmm1,a.xmm1);
	__m128i t2 = _mm_add_epi8(a.xmm2,a.xmm2);
	__m128i t3 = _mm_add_epi8(a.xmm3,a.xmm3);
	a.xmm0 = _mm_add_epi8(t0,t0)^_mm_shuffle_epi8( tab_h , _mm_srli_epi16(a.xmm0&mask_67bit,4) );
	a.xmm1 = _mm_add_epi8(t1,t1)^_mm_shuffle_epi8( tab_h , _mm_srli_epi16(a.xmm1&mask_67bit,4) );
	a.xmm2 = _mm_add_epi8(t2,t2)^_mm_shuffle_epi8( tab_h , _mm_srli_epi16(a.xmm2&mask_67bit,4) );
	a.xmm3 = _mm_add_epi8(t3,t3)^_mm_shuffle_epi8( tab_h , _mm_srli_epi16(a.xmm3&mask_67bit,4) );
#endif
#endif
}

// alpha3 0x08
inline void mul_alpha3( xmm_x4 & a ) {
	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i & tab_h = * ((__m128i *)(&sse_mul_tab_h[8*16]));
	a.xmm0 = _mm_slli_epi32(a.xmm0&mask_low,3)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm0,4)&mask_low );
	a.xmm1 = _mm_slli_epi32(a.xmm1&mask_low,3)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm1,4)&mask_low );
	a.xmm2 = _mm_slli_epi32(a.xmm2&mask_low,3)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm2,4)&mask_low );
	a.xmm3 = _mm_slli_epi32(a.xmm3&mask_low,3)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm3,4)&mask_low );
}

// alpha4 0x10
inline void mul_alpha4( xmm_x4 & a ) {
	__m128i mask_low = _mm_load_si128( (__m128i*) _mask_low );
//	__m128i & mask_low = * ((__m128i *)(&_mask_low[0]));
	__m128i & tab_h = * ((__m128i *)(&sse_mul_tab_h[16*16]));
	a.xmm0 = _mm_slli_epi32(a.xmm0&mask_low,4)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm0,4)&mask_low );
	a.xmm1 = _mm_slli_epi32(a.xmm1&mask_low,4)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm1,4)&mask_low );
	a.xmm2 = _mm_slli_epi32(a.xmm2&mask_low,4)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm2,4)&mask_low );
	a.xmm3 = _mm_slli_epi32(a.xmm3&mask_low,4)^_mm_shuffle_epi8( tab_h , _mm_srli_epi32(a.xmm3,4)&mask_low );
}



#endif // _NO_SSE_


////////////////////////////////////////////////////////////////////


#ifdef _NO_AVX2_

typedef __m128i ymm_x1;
typedef xmm_x2 ymm_x2;
typedef xmm_x4 ymm_x4;

#else

#include <immintrin.h>

inline void and_not_ymm( __m256i & a , __m256i b )
{
	a = _mm256_andnot_si256(a,b);
}

inline __m256i gfv_mul_ymm( __m256i a , uint8_t b ) {
	__m256i & mask_low = * (__m256i *)_mask_low;
#if 0
	__m128i & tab_l = ((__m128i *)(&sse_mul_tab_l[0]))[b];
	__m128i & tab_h = ((__m128i *)(&sse_mul_tab_h[0]))[b];
	__m256i tb_l = _mm256_castsi128_si256(tab_l); tb_l = _mm256_insertf128_si256(tb_l,tab_l,1);
	__m256i tb_h = _mm256_castsi128_si256(tab_h); tb_h = _mm256_insertf128_si256(tb_h,tab_h,1);
#else
	__m256i tb_h = _mm256_load_si256( & ((__m256i *)(avx_mul_tab))[((uint16_t)b)<<1]  );
	__m256i tb_l = _mm256_load_si256( & ((__m256i *)(avx_mul_tab))[(((uint16_t)b)<<1)+1]  );
#endif
	return _mm256_shuffle_epi8( tb_l, a&mask_low )^_mm256_shuffle_epi8( tb_h , _mm256_srli_epi32(a,4)&mask_low );
}

// alpha 0x02
inline void mul_alpha_ymm( __m256i & a ) {
	__m256i & ppoly = * (__m256i *)_pri_poly;
	a = _mm256_add_epi8(a,a) ^ ppoly ^ _mm256_shuffle_epi8( ppoly , a );
}

// alpha2 0x04
inline void mul_alpha2_ymm( __m256i & a ) {
	__m256i & tab_h = ((__m256i *)avx_mul_tab)[8];
#ifdef USE_BIT_SHIFT
	__m256i & mask_low = * (__m256i *) _mask_low ;
	a = _mm256_slli_epi16(a&mask_low,2)^_mm256_shuffle_epi8(tab_h,_mm256_srli_epi32(a,4)&mask_low);
#else
	__m256i & mask_67bit = * (__m256i*) _mask_67bit ;
	__m256i t = _mm256_add_epi8(a,a);
	a = _mm256_add_epi8(t,t)^_mm256_shuffle_epi8( tab_h , _mm256_srli_epi16(a&mask_67bit,4) );
#endif
}

// alpha3 0x08
inline void mul_alpha3_ymm( __m256i & a ) {
	__m256i & mask_low = * (__m256i *) _mask_low ;
	__m256i & tab_h = ((__m256i *)avx_mul_tab)[16];
	a = _mm256_slli_epi16(a&mask_low,3)^_mm256_shuffle_epi8(tab_h,_mm256_srli_epi32(a,4)&mask_low);
}

inline void mul_sqrt_alpha_ymm( __m256i & a ) {
	__m256i & mask_low = * (__m256i *)_mask_low;
	__m256i & tab_h = ((__m256i *)avx_mul_tab)[SQRT_ALPHA*2];
	__m256i & tab_l = ((__m256i *)avx_mul_tab)[SQRT_ALPHA*2+1];
	a = _mm256_shuffle_epi8( tab_l, a&mask_low )^_mm256_shuffle_epi8( tab_h , _mm256_srli_epi32(a,4)&mask_low );
}


/////////////////////////////////////////////////////

struct ymm_x1 {
	__m256i ymm0;
};

inline const ymm_x1 operator^( const ymm_x1 & a , const ymm_x1 & b ) {
	ymm_x1 ret;
	ret.ymm0 = a.ymm0^b.ymm0;
	return ret;
}

inline ymm_x1 & operator^=( ymm_x1 & a , const ymm_x1 & b) {
	a.ymm0^=b.ymm0;
	return a;
}

inline ymm_x1 & operator&=( ymm_x1 & a , const ymm_x1 & b) {
	a.ymm0 &= b.ymm0;
	return a;
}

inline ymm_x1 & operator|=( ymm_x1 & a , const ymm_x1 & b) {
	a.ymm0 |= b.ymm0;
	return a;
}

inline void and_not( ymm_x1 & a , const ymm_x1 & b ) {
	and_not_ymm(a.ymm0,b.ymm0);
}



inline const ymm_x1 gfv_mul( const ymm_x1 & a , uint8_t b ) {
	ymm_x1 r;
	r.ymm0 = gfv_mul_ymm(a.ymm0,b);
	return r;
}

inline void mul_alpha( ymm_x1 & a ) {
	mul_alpha_ymm(a.ymm0);
}

inline void mul_alpha2( ymm_x1 & a ) {
	mul_alpha2_ymm(a.ymm0);
}

inline void mul_alpha3( ymm_x1 & a ) {
	mul_alpha3_ymm(a.ymm0);
}

inline void mul_sqrt_alpha( ymm_x1 & a ) {
	mul_sqrt_alpha_ymm(a.ymm0);
}


/////////////////////////////////////////////////////

struct ymm_x2 {
	__m256i ymm0;
	__m256i ymm1;
};

inline const ymm_x2 operator^( const ymm_x2 & a , const ymm_x2 & b ) {
	ymm_x2 ret;
	ret.ymm0 = a.ymm0^b.ymm0;
	ret.ymm1 = a.ymm1^b.ymm1;
	return ret;
}

inline ymm_x2 & operator^=( ymm_x2 & a , const ymm_x2 & b) {
	a.ymm0^=b.ymm0;
	a.ymm1^=b.ymm1;
	return a;
}

inline ymm_x2 & operator&=( ymm_x2 & a , const ymm_x2 & b) {
	a.ymm0 &= b.ymm0;
	a.ymm1 &= b.ymm1;
	return a;
}

inline ymm_x2 & operator|=( ymm_x2 & a , const ymm_x2 & b) {
	a.ymm0 |= b.ymm0;
	a.ymm1 |= b.ymm1;
	return a;
}

inline void and_not( ymm_x2 & a , const ymm_x2 & b ) {
	and_not_ymm(a.ymm0,b.ymm0);
	and_not_ymm(a.ymm1,b.ymm1);
}



inline const ymm_x2 gfv_mul( const ymm_x2 & a , uint8_t b ) {
	ymm_x2 r;
	r.ymm0 = gfv_mul_ymm(a.ymm0,b);
	r.ymm1 = gfv_mul_ymm(a.ymm1,b);
	return r;
}

inline void mul_alpha( ymm_x2 & a ) {
	mul_alpha_ymm(a.ymm0);
	mul_alpha_ymm(a.ymm1);
}

inline void mul_alpha2( ymm_x2 & a ) {
	mul_alpha2_ymm(a.ymm0);
	mul_alpha2_ymm(a.ymm1);
}

inline void mul_alpha3( ymm_x2 & a ) {
	mul_alpha3_ymm(a.ymm0);
	mul_alpha3_ymm(a.ymm1);
}

inline void mul_sqrt_alpha( ymm_x2 & a ) {
	mul_sqrt_alpha_ymm(a.ymm0);
	mul_sqrt_alpha_ymm(a.ymm1);
}





/////////////////////////////////////////////////////

struct ymm_x4 {
	__m256i ymm0;
	__m256i ymm1;
	__m256i ymm2;
	__m256i ymm3;
};

inline const ymm_x4 operator^( const ymm_x4 & a , const ymm_x4 & b ) {
	ymm_x4 ret;
	ret.ymm0 = a.ymm0^b.ymm0;
	ret.ymm1 = a.ymm1^b.ymm1;
	ret.ymm2 = a.ymm2^b.ymm2;
	ret.ymm3 = a.ymm3^b.ymm3;
	return ret;
}

inline ymm_x4 & operator^=( ymm_x4 & a , const ymm_x4 & b) {
	a.ymm0^=b.ymm0;
	a.ymm1^=b.ymm1;
	a.ymm2^=b.ymm2;
	a.ymm3^=b.ymm3;
	return a;
}

inline ymm_x4 & operator&=( ymm_x4 & a , const ymm_x4 & b) {
	a.ymm0 &= b.ymm0;
	a.ymm1 &= b.ymm1;
	a.ymm2 &= b.ymm2;
	a.ymm3 &= b.ymm3;
	return a;
}

inline ymm_x4 & operator|=( ymm_x4 & a , const ymm_x4 & b) {
	a.ymm0 |= b.ymm0;
	a.ymm1 |= b.ymm1;
	a.ymm2 |= b.ymm2;
	a.ymm3 |= b.ymm3;
	return a;
}

inline void and_not( ymm_x4 & a , const ymm_x4 & b ) {
	and_not_ymm(a.ymm0,b.ymm0);
	and_not_ymm(a.ymm1,b.ymm1);
	and_not_ymm(a.ymm2,b.ymm2);
	and_not_ymm(a.ymm3,b.ymm3);
}




inline const ymm_x4 gfv_mul( const ymm_x4 & a , uint8_t b ) {
	ymm_x4 r;
	r.ymm0 = gfv_mul_ymm(a.ymm0,b);
	r.ymm1 = gfv_mul_ymm(a.ymm1,b);
	r.ymm2 = gfv_mul_ymm(a.ymm2,b);
	r.ymm3 = gfv_mul_ymm(a.ymm3,b);
	return r;
}

inline void mul_alpha( ymm_x4 & a ) {
	mul_alpha_ymm(a.ymm0);
	mul_alpha_ymm(a.ymm1);
	mul_alpha_ymm(a.ymm2);
	mul_alpha_ymm(a.ymm3);
}

inline void mul_alpha2( ymm_x4 & a ) {
	mul_alpha2_ymm(a.ymm0);
	mul_alpha2_ymm(a.ymm1);
	mul_alpha2_ymm(a.ymm2);
	mul_alpha2_ymm(a.ymm3);
}

inline void mul_alpha3( ymm_x4 & a ) {
	mul_alpha3_ymm(a.ymm0);
	mul_alpha3_ymm(a.ymm1);
	mul_alpha3_ymm(a.ymm2);
	mul_alpha3_ymm(a.ymm3);
}

inline void mul_sqrt_alpha( ymm_x4 & a ) {
	mul_sqrt_alpha_ymm(a.ymm0);
	mul_sqrt_alpha_ymm(a.ymm1);
	mul_sqrt_alpha_ymm(a.ymm2);
	mul_sqrt_alpha_ymm(a.ymm3);
}




#endif /// _NO_AVX2_

#endif
