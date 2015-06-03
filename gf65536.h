#ifndef _GF65536_H_
#define _GF65536_H_


#include "gf256.h"

//
// GF(65536)
// F216 := F8[X]/X^2 + a^3 X + 1
//


inline uint16_t gf216_mul( uint16_t a , uint16_t b )
{
	union poly { uint16_t v; uint8_t _v[2]; } _a,_b,_c;
	_a.v = a;
	_b.v = b;
//	_c.v = 0;
//	uint8_t v2 = gf_mul( _a._v[1] , _b._v[1] );
//	_c._v[0] = gf_mul( _a._v[0] , _b._v[0] ) ^ v2;
//	_c._v[1] = gf_mul( _a._v[1] , _b._v[0] ) ^ gf_mul( _a._v[0] , _b._v[1] ) ^ gf_mul( v2 , 0x08 );
	uint8_t a1b1 = gf_mul( _a._v[1] , _b._v[1] );
	_c._v[0] = gf_mul( _a._v[0] , _b._v[0] ) ^ a1b1;
	_c._v[1] = gf_mul( a1b1 , 0x08 ) ^ _c._v[0] ^ gf_mul( _a._v[0]^_a._v[1] , _b._v[0]^_b._v[1] );

	return _c.v;
}

inline uint16_t gf216_mul_gf256( uint16_t a , uint8_t b )
{
	union poly { uint16_t v; uint8_t _v[2]; } _a,_c;
	_a.v = a;
	_c._v[0] = gf_mul( _a._v[0] , b );
	_c._v[1] = gf_mul( _a._v[1] , b );
	return _c.v;
}


inline uint16_t gf216_inv( uint16_t a )
{
	if( 0 == a ) return 0; /// exception!!!
	union poly { uint16_t v; uint8_t _v[2]; } _a,_b;
	_a.v = a;
	if( 0 == _a._v[1] ) return gf_inv( _a._v[0] );
	if( 0 == _a._v[0] ) {
		_b._v[1] = gf_inv( _a._v[1] );
		_b._v[0] = gf_mul( _b._v[1] , 0x08 );
		return _b.v;
	}
	uint8_t deno = gf_mul(gf_mul( _a._v[1] , _a._v[0] ) , 0x08 )^gf_mul(_a._v[1],_a._v[1])^gf_mul(_a._v[0],_a._v[0]);
	_b._v[1] = gf_mul( _a._v[1] , gf_inv( deno ) );
	_b._v[0] = gf_mul( gf_inv(_a._v[0]) , 1^gf_mul(_a._v[1],_b._v[1]) );
	return _b.v;
}


#ifndef _NO_SSE_

#include "gf256_xmm.h"

inline const __m128i gf216v_mul( const __m128i & a , uint16_t b ) {
	union poly { uint16_t v; uint8_t _v[2]; } _b;
	_b.v = b;
	__m128i & mask_low_byte = * ((__m128i *)(&_low_byte_epi16[0]));
	__m128i a0 = a & mask_low_byte;
	__m128i a1 = _mm_andnot_si128( mask_low_byte , a );

	__m128i a0b0 = gfv_mul(a0,_b._v[0]);
	__m128i a1b1 = gfv_mul(a1,_b._v[1]);
	__m128i a0b1pa1b0 = gfv_mul( a1 ^ _mm_slli_si128(a0,1) , _b._v[0]^_b._v[1] ) ^ _mm_slli_si128(a0b0,1) ^ a1b1;

	a0b0 ^= _mm_srli_si128(a1b1,1);
	mul_alpha3( a1b1 );
	return a0b1pa1b0 ^ a0b0 ^ a1b1;
}

//inline const __m128i gf216v_mul_0x0100( const __m128i & a ) {
//	__m128i & mask_low_byte = * ((__m128i *)(&_low_byte_epi16[0]));
//	__m128i low_byte = a & mask_low_byte;
//	__m128i high_byte = _mm_andnot_si128( mask_low_byte , a );
//	low_byte = _mm_slli_si128(low_byte,1)^_mm_srli_si128(high_byte,1);
//	mul_alpha3( high_byte );
//	return high_byte^low_byte;
//}
inline void gf216v_mul_0x0100( __m128i & a ) {
	__m128i & mask_low_byte = * ((__m128i *)(&_low_byte_epi16[0]));
	__m128i low_byte = a & mask_low_byte;
	__m128i high_byte = _mm_andnot_si128( mask_low_byte , a );
	low_byte = _mm_slli_si128(low_byte,1)^_mm_srli_si128(high_byte,1);
	mul_alpha3( high_byte );
	a = high_byte^low_byte;
}

////////////////////////////////////


inline const xmm_x2 gf216bsv_mul( const xmm_x2 & a , uint16_t b ) {
	union poly { uint16_t v; uint8_t _v[2]; } _b; _b.v = b;
	xmm_x2 ret;
	__m128i a0b0 = gfv_mul(a.xmm0,_b._v[0]);
	__m128i a1b1 = gfv_mul(a.xmm1,_b._v[1]);
	a0b0 ^= a1b1;
	mul_alpha3( a1b1 );
	ret.xmm1 = gfv_mul( a.xmm0 ^ a.xmm1 , _b._v[0]^_b._v[1] ) ^ a0b0 ^ a1b1;
	ret.xmm0 = a0b0;
	return ret;
}

inline void gf216bsv_mul_0x0100( xmm_x2 & a ) {
	__m128i a1 = a.xmm1;
	a.xmm1 = a.xmm0;
	a.xmm0 = a1;
	mul_alpha3( a1 );
	a.xmm1 ^= a1;
}


/// a^2X+1
/// ((a^5 + 1)b1 + a^2 b0 )X + a^2 b1 + b0
///  ( 0x21 a1 + 0x4 a0 ) X + 0x4 a1 + a0
inline void gf216bsv_mul_0x0401( xmm_x2 & a ) {
	__m128i a1x4 = a.xmm1;
	mul_alpha2( a1x4 );
	__m128i a0x4 = a.xmm0;
	mul_alpha2( a0x4 );
	a.xmm0 ^= a1x4;
	a.xmm1 = a0x4 ^ gfv_mul( a.xmm1 , 0x21 );
}

/// a^141X
/// (a1X+a0) x a^141X
/// = a^141 a1 X^2 + a^141 a0 X = ( a^144 a1 + a^141 a0 ) X + a^141 a1
/// a^141 = 0x15     a^144 = 0xa8
inline void gf216bsv_mul_0x1500( xmm_x2 & a ) {
	__m128i a0 = a.xmm0;
	__m128i a1 = a.xmm1;
	a.xmm1 = gfv_mul( a0 , 0x15 )^gfv_mul( a1 , 0xa8 );
	a.xmm0 = gfv_mul( a1 , 0x15 );
}

/// a^186X+a^6
/// (a1X+a0) x (a^186X + a^6)
inline void gf216bsv_mul_0x6e40( xmm_x2 & a ) {
	a = gf216bsv_mul( a , 0x6e40 );
}

inline const xmm_x2 gf216v_mul( const xmm_x2 & a , uint16_t b ) {
	xmm_x2 r;
	r.xmm0 = gf216v_mul( a.xmm0 , b );
	r.xmm1 = gf216v_mul( a.xmm1 , b );
	return r;
}

inline void gf216v_mul_0x0100( xmm_x2 & a ) {
	gf216v_mul_0x0100( a.xmm0 );
	gf216v_mul_0x0100( a.xmm1 );
}

//////////////////////////////////////

inline const xmm_x4 gf216bsv_mul( const xmm_x4 & a , uint16_t b ) {
	union poly { uint16_t v; uint8_t _v[2]; } _b; _b.v = b;
	xmm_x4 ret;
	__m128i a0b0 = gfv_mul(a.xmm0,_b._v[0]);
	__m128i a1b1 = gfv_mul(a.xmm1,_b._v[1]);
	a0b0 ^= a1b1;
	mul_alpha3( a1b1 );
	ret.xmm1 = gfv_mul( a.xmm0 ^ a.xmm1 , _b._v[0]^_b._v[1] ) ^ a0b0 ^ a1b1;
	ret.xmm0 = a0b0;
/////////////////////////
	a0b0 = gfv_mul(a.xmm2,_b._v[0]);
	a1b1 = gfv_mul(a.xmm3,_b._v[1]);
	a0b0 ^= a1b1;
	mul_alpha3( a1b1 );
	ret.xmm3 = gfv_mul( a.xmm2 ^ a.xmm3 , _b._v[0]^_b._v[1] ) ^ a0b0 ^ a1b1;
	ret.xmm2 = a0b0;
	return ret;
}

inline void gf216bsv_mul_0x0100( xmm_x4 & a ) {
	__m128i a1 = a.xmm1;
	a.xmm1 = a.xmm0;
	a.xmm0 = a1;
	mul_alpha3( a1 );
	a.xmm1 ^= a1;
//////////////
	a1 = a.xmm3;
	a.xmm3 = a.xmm2;
	a.xmm2 = a1;
	mul_alpha3( a1 );
	a.xmm3 ^= a1;
}


/// a^2X+1
/// ((a^5 + 1)b1 + a^2 b0 )X + a^2 b1 + b0
///  ( 0x21 a1 + 0x4 a0 ) X + 0x4 a1 + a0
inline void gf216bsv_mul_0x0401( xmm_x4 & a ) {
	__m128i a1x4 = a.xmm1;
	mul_alpha2( a1x4 );
	__m128i a0x4 = a.xmm0;
	mul_alpha2( a0x4 );
	a.xmm0 ^= a1x4;
	a.xmm1 = a0x4 ^ gfv_mul( a.xmm1 , 0x21 );
/////////////////
	a1x4 = a.xmm3;
	mul_alpha2( a1x4 );
	a0x4 = a.xmm2;
	mul_alpha2( a0x4 );
	a.xmm2 ^= a1x4;
	a.xmm3 = a0x4 ^ gfv_mul( a.xmm3 , 0x21 );
}

/// a^141X
/// (a1X+a0) x a^141X
/// = a^141 a1 X^2 + a^141 a0 X = ( a^144 a1 + a^141 a0 ) X + a^141 a1
/// a^141 = 0x15     a^144 = 0xa8
inline void gf216bsv_mul_0x1500( xmm_x4 & a ) {
	__m128i a0 = a.xmm0;
	__m128i a1 = a.xmm1;
	a.xmm1 = gfv_mul( a0 , 0x15 )^gfv_mul( a1 , 0xa8 );
	a.xmm0 = gfv_mul( a1 , 0x15 );
//////////////////
	a0 = a.xmm2;
	a1 = a.xmm3;
	a.xmm3 = gfv_mul( a0 , 0x15 )^gfv_mul( a1 , 0xa8 );
	a.xmm2 = gfv_mul( a1 , 0x15 );
}

/// a^186X+a^6
/// (a1X+a0) x (a^186X + a^6)
inline void gf216bsv_mul_0x6e40( xmm_x4 & a ) {
	a = gf216bsv_mul( a , 0x6e40 );
}



inline const xmm_x4 gf216v_mul( const xmm_x4 & a , uint16_t b ) {
	xmm_x4 r;
	r.xmm0 = gf216v_mul( a.xmm0 , b );
	r.xmm1 = gf216v_mul( a.xmm1 , b );
	r.xmm2 = gf216v_mul( a.xmm2 , b );
	r.xmm3 = gf216v_mul( a.xmm3 , b );
	return r;
}

inline void gf216v_mul_0x0100( xmm_x4 & a ) {
	gf216v_mul_0x0100( a.xmm0 );
	gf216v_mul_0x0100( a.xmm1 );
	gf216v_mul_0x0100( a.xmm2 );
	gf216v_mul_0x0100( a.xmm3 );
}




//////////////////////////////////////



#ifndef _NO_AVX2_

inline const ymm_x2 gf216bsv_mul( const ymm_x2 & a , uint16_t b ) {
	union poly { uint16_t v; uint8_t _v[2]; } _b; _b.v = b;
	ymm_x2 ret;
	__m256i a0b0 = gfv_mul_ymm(a.ymm0,_b._v[0]);
	__m256i a1b1 = gfv_mul_ymm(a.ymm1,_b._v[1]);
	a0b0 ^= a1b1;
	mul_alpha3_ymm( a1b1 );
	ret.ymm1 = gfv_mul_ymm( a.ymm0 ^ a.ymm1 , _b._v[0]^_b._v[1] ) ^ a0b0 ^ a1b1;
	ret.ymm0 = a0b0;
	return ret;
}

inline void gf216bsv_mul_0x0100( ymm_x2 & a ) {
	__m256i a1 = a.ymm1;
	a.ymm1 = a.ymm0;
	a.ymm0 = a1;
	mul_alpha3_ymm( a1 );
	a.ymm1 ^= a1;
}


/// a^2X+1
/// ((a^5 + 1)b1 + a^2 b0 )X + a^2 b1 + b0
///  ( 0x21 a1 + 0x4 a0 ) X + 0x4 a1 + a0
inline void gf216bsv_mul_0x0401( ymm_x2 & a ) {
	__m256i a1x4 = a.ymm1;
	mul_alpha2_ymm( a1x4 );
	__m256i a0x4 = a.ymm0;
	mul_alpha2_ymm( a0x4 );
	a.ymm0 ^= a1x4;
	a.ymm1 = a0x4 ^ gfv_mul_ymm( a.ymm1 , 0x21 );
}

/// a^141X
/// (a1X+a0) x a^141X
/// = a^141 a1 X^2 + a^141 a0 X = ( a^144 a1 + a^141 a0 ) X + a^141 a1
/// a^141 = 0x15     a^144 = 0xa8
inline void gf216bsv_mul_0x1500( ymm_x2 & a ) {
	__m256i a0 = a.ymm0;
	__m256i a1 = a.ymm1;
	a.ymm1 = gfv_mul_ymm( a0 , 0x15 )^gfv_mul_ymm( a1 , 0xa8 );
	a.ymm0 = gfv_mul_ymm( a1 , 0x15 );
}

/// a^186X+a^6
/// (a1X+a0) x (a^186X + a^6)
inline void gf216bsv_mul_0x6e40( ymm_x2 & a ) {
	a = gf216bsv_mul( a , 0x6e40 );
}








//////////////////////////////////////





inline const ymm_x4 gf216bsv_mul( const ymm_x4 & a , uint16_t b ) {
	union poly { uint16_t v; uint8_t _v[2]; } _b; _b.v = b;
	ymm_x4 ret;
	__m256i a0b0 = gfv_mul_ymm(a.ymm0,_b._v[0]);
	__m256i a1b1 = gfv_mul_ymm(a.ymm1,_b._v[1]);
	a0b0 ^= a1b1;
	mul_alpha3_ymm( a1b1 );
	ret.ymm1 = gfv_mul_ymm( a.ymm0 ^ a.ymm1 , _b._v[0]^_b._v[1] ) ^ a0b0 ^ a1b1;
	ret.ymm0 = a0b0;
/////////////////////////
	a0b0 = gfv_mul_ymm(a.ymm2,_b._v[0]);
	a1b1 = gfv_mul_ymm(a.ymm3,_b._v[1]);
	a0b0 ^= a1b1;
	mul_alpha3_ymm( a1b1 );
	ret.ymm3 = gfv_mul_ymm( a.ymm2 ^ a.ymm3 , _b._v[0]^_b._v[1] ) ^ a0b0 ^ a1b1;
	ret.ymm2 = a0b0;
	return ret;
}

inline void gf216bsv_mul_0x0100( ymm_x4 & a ) {
	__m256i a1 = a.ymm1;
	a.ymm1 = a.ymm0;
	a.ymm0 = a1;
	mul_alpha3_ymm( a1 );
	a.ymm1 ^= a1;
//////////////
	a1 = a.ymm3;
	a.ymm3 = a.ymm2;
	a.ymm2 = a1;
	mul_alpha3_ymm( a1 );
	a.ymm3 ^= a1;
}


/// a^2X+1
/// ((a^5 + 1)b1 + a^2 b0 )X + a^2 b1 + b0
///  ( 0x21 a1 + 0x4 a0 ) X + 0x4 a1 + a0
inline void gf216bsv_mul_0x0401( ymm_x4 & a ) {
	__m256i a1x4 = a.ymm1;
	mul_alpha2_ymm( a1x4 );
	__m256i a0x4 = a.ymm0;
	mul_alpha2_ymm( a0x4 );
	a.ymm0 ^= a1x4;
	a.ymm1 = a0x4 ^ gfv_mul_ymm( a.ymm1 , 0x21 );
/////////////////
	a1x4 = a.ymm3;
	mul_alpha2_ymm( a1x4 );
	a0x4 = a.ymm2;
	mul_alpha2_ymm( a0x4 );
	a.ymm2 ^= a1x4;
	a.ymm3 = a0x4 ^ gfv_mul_ymm( a.ymm3 , 0x21 );
}

/// a^141X
/// (a1X+a0) x a^141X
/// = a^141 a1 X^2 + a^141 a0 X = ( a^144 a1 + a^141 a0 ) X + a^141 a1
/// a^141 = 0x15     a^144 = 0xa8
inline void gf216bsv_mul_0x1500( ymm_x4 & a ) {
	__m256i a0 = a.ymm0;
	__m256i a1 = a.ymm1;
	a.ymm1 = gfv_mul_ymm( a0 , 0x15 )^gfv_mul_ymm( a1 , 0xa8 );
	a.ymm0 = gfv_mul_ymm( a1 , 0x15 );
//////////////////
	a0 = a.ymm2;
	a1 = a.ymm3;
	a.ymm3 = gfv_mul_ymm( a0 , 0x15 )^gfv_mul_ymm( a1 , 0xa8 );
	a.ymm2 = gfv_mul_ymm( a1 , 0x15 );
}

/// a^186X+a^6
/// (a1X+a0) x (a^186X + a^6)
inline void gf216bsv_mul_0x6e40( ymm_x4 & a ) {
	a = gf216bsv_mul( a , 0x6e40 );
}


#endif /// _NO_AVX2_




/////////////////////////////////////////

#endif

#endif
