#ifndef _GF256_H_
#define _GF256_H_


//#ifndef __KERNEL__

#if 1
#include <stdint.h>
#else
#ifndef _D_U8_
#define _D_U8_
typedef unsigned char uint8_t;
#endif
#ifndef _D_U16_
#define _D_U16_
typedef unsigned short uint16_t;
#endif
#ifndef _D_U32_
#define _D_U32_
typedef unsigned int uint32_t;
#endif
#ifndef _D_U64_
#define _D_U64_
typedef unsigned long long uint64_t;
#endif
#endif

//#endif

#ifdef __cplusplus
/*
* GF(256)
* primitive polynomail x^8 + x^4 + x^3 + x^2 + 1
* 0x11d
*/
extern "C" {
#endif

extern const uint8_t gf256_log_tab[];
extern const uint8_t gf256_alog_tab[];
extern const uint8_t gf256_inv_tab[];

extern const uint8_t sse_mul_tab_l[];
extern const uint8_t sse_mul_tab_h[];

extern const uint32_t _mask_brvs[];
extern const uint32_t _pri_poly[];
extern const uint32_t _brvs_irr_poly[];
extern const uint32_t _brvs_9thbit[];
extern const uint32_t _mask_67bit[];
extern const uint32_t _msb_epi8[];
extern const uint32_t _mask_low[];

extern const uint32_t _low_byte_epi16[];


extern const uint8_t sse_mul_tab[];
extern const uint8_t avx_mul_tab[];


#ifdef __cplusplus
}
#endif



static inline uint8_t gf_inv( uint8_t a ) { return gf256_inv_tab[a]; }

static inline uint16_t gf_log( uint8_t a ) { return (uint16_t)gf256_log_tab[a]; }

static inline uint8_t gf_alog( uint16_t a) { return gf256_alog_tab[a]; }

static inline uint8_t gf_mul( uint8_t a , uint8_t b ) { if((0==a)||(0==b)) return 0; return gf_alog(gf_log(a)+gf_log(b)); }

static inline uint8_t gf_mula( uint8_t c) { return (c<<1)^((c&0x80)?0x1d:0); }

/* sqrt alpha: 0x85   1000,0101 */
#define SQRT_ALPHA 0x85

/* alpha^(3/2): 0x17   0001,0111 */
#define ALPHA_3O2 0x17





#endif
