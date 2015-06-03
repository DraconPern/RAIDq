#ifndef _RECOVER_H_
#define _RECOVER_H_



typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#ifdef __cplusplus
extern "C" {
#endif

void get_rec_coef2( uint8_t *rec_coe[] , int a, int b, const uint8_t * coef[] );

void get_rec_coef2_xor( uint8_t *rec_coe[] , int a, int b, const uint8_t * coef[] );

void get_rec_coef3( uint8_t *rec_coe[] , int a, int b, int c, const uint8_t * coef[] );

void get_rec_coef3_xor( uint8_t *rec_coe[] , int a, int b, int c, const uint8_t * coef[] );

void get_rec_coef4( uint8_t *rec_coe[] , uint8_t idx[], const uint8_t * coef[] );

void get_rec_coef4_xor_gf216( uint16_t *rec_coe[] , uint8_t idx[], const uint8_t * coef[] , const uint16_t coef216[] );



#ifdef __cplusplus
}
#endif

#endif
