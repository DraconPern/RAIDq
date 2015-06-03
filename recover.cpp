
#include "recover.h"
#include "gf256.h"


void get_rec_coef2( uint8_t *rec_coe[] , int a, int b, const uint8_t * coef[] )
{
	uint8_t m11 = coef[0][a];
	uint8_t m12 = coef[0][b];
	uint8_t m21 = coef[1][a];
	uint8_t m22 = coef[1][b];

        uint8_t inv_m11 = gf_inv(m11);
        uint8_t m12divm11 = gf_mul(m12,inv_m11);
        uint8_t inv_m22etc = gf_inv( m22 ^ gf_mul(m12divm11,m21) );
	uint8_t m21divm11 = gf_mul(m21,inv_m11);

	rec_coe[1][0] = gf_mul( m21divm11 , inv_m22etc );
	rec_coe[1][1] = inv_m22etc;
	rec_coe[0][1] = gf_mul( m12divm11 , inv_m22etc );
	rec_coe[0][0] = inv_m11 ^ gf_mul( m12divm11 , rec_coe[1][0] );

}


void get_rec_coef2_xor( uint8_t *rec_coe[] , int a, int b, const uint8_t * coef[] )
{
//	uint8_t m11 = 1;
//	uint8_t m12 = 1;
	uint8_t m21 = coef[1][a];
	uint8_t m22 = coef[1][b];

//        uint8_t inv_m11 = 1;
//        uint8_t m12divm11 = 1;
        uint8_t inv_m22etc = gf_inv( m22 ^ m21 );
	uint8_t m21divm11 = m21;

	rec_coe[1][0] = gf_mul( m21divm11 , inv_m22etc );
	rec_coe[1][1] = inv_m22etc;
	rec_coe[0][1] = inv_m22etc;
	rec_coe[0][0] = 1 ^ rec_coe[1][0];
}





//static void get_inv_mat3_xor( uint8_t mat3[3][4] )
static void get_inv_mat3_xor( uint8_t * mat3[] )
{
	uint32_t * r0 = (uint32_t*)& mat3[0][0];
	uint32_t * r1 = (uint32_t*)& mat3[1][0];
	uint32_t * r2 = (uint32_t*)& mat3[2][0];

	uint8_t col[3];
	uint8_t inv;
	union{
	uint8_t tmpr8[4];
	uint32_t tmpr;
	} u;

	//col[0] = 1;
	col[1] = mat3[1][0];
	col[2] = mat3[2][0];
	//mat3[0][0] = 1;
	mat3[1][0] = 0;
	mat3[2][0] = 0;
	//inv = gf_inv( col[0] );
	//mat3[0][0] = inv;
	//mat3[0][1] = mat3[0][1];
	//mat3[0][2] = mat3[0][2];
	u.tmpr8[0] = col[1];
	u.tmpr8[1] = col[1];
	u.tmpr8[2] = col[1];
	r1[0] ^= u.tmpr;
	u.tmpr8[0] = col[2];
	u.tmpr8[1] = col[2];
	u.tmpr8[2] = col[2];
	r2[0] ^= u.tmpr;
/////////////
	//col[0] = mat3[0][1];
	col[0] = 1;
	col[1] = mat3[1][1];
	col[2] = mat3[2][1];
	mat3[0][1] = 0;
	mat3[1][1] = 1;
	mat3[2][1] = 0;
	inv = gf_inv( col[1] );
	mat3[1][0] = gf_mul( mat3[1][0] , inv );
	mat3[1][1] = inv;
	mat3[1][2] = gf_mul( mat3[1][2] , inv );
	//u.tmpr8[0] = gf_mul( mat3[1][0] , col[0] );
	//u.tmpr8[1] = gf_mul( mat3[1][1] , col[0] );
	//u.tmpr8[2] = gf_mul( mat3[1][2] , col[0] );
	u.tmpr8[0] = mat3[1][0];
	u.tmpr8[1] = mat3[1][1];
	u.tmpr8[2] = mat3[1][2];
	r0[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat3[1][0] , col[2] );
	u.tmpr8[1] = gf_mul( mat3[1][1] , col[2] );
	u.tmpr8[2] = gf_mul( mat3[1][2] , col[2] );
	r2[0] ^= u.tmpr;
//////////////
	col[0] = mat3[0][2];
	col[1] = mat3[1][2];
	col[2] = mat3[2][2];
	mat3[0][2] = 0;
	mat3[1][2] = 0;
	mat3[2][2] = 1;
	inv = gf_inv( col[2] );
	mat3[2][0] = gf_mul( mat3[2][0] , inv );
	mat3[2][1] = gf_mul( mat3[2][1] , inv );
	mat3[2][2] = inv;
	u.tmpr8[0] = gf_mul( mat3[2][0] , col[0] );
	u.tmpr8[1] = gf_mul( mat3[2][1] , col[0] );
	u.tmpr8[2] = gf_mul( mat3[2][2] , col[0] );
	r0[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat3[2][0] , col[1] );
	u.tmpr8[1] = gf_mul( mat3[2][1] , col[1] );
	u.tmpr8[2] = gf_mul( mat3[2][2] , col[1] );
	r1[0] ^= u.tmpr;

}



//static void get_inv_mat3( uint8_t mat3[3][4] )
static void get_inv_mat3( uint8_t * mat3[] )
{
	uint32_t * r0 = (uint32_t*)& mat3[0][0];
	uint32_t * r1 = (uint32_t*)& mat3[1][0];
	uint32_t * r2 = (uint32_t*)& mat3[2][0];

	uint8_t col[3];
	uint8_t inv;
	union{
	uint8_t tmpr8[4];
	uint32_t tmpr;
	} u;

	col[0] = mat3[0][0];
	col[1] = mat3[1][0];
	col[2] = mat3[2][0];
	mat3[0][0] = 1;
	mat3[1][0] = 0;
	mat3[2][0] = 0;
	inv = gf_inv( col[0] );
	mat3[0][0] = inv;
	mat3[0][1] = gf_mul( mat3[0][1] , inv );
	mat3[0][2] = gf_mul( mat3[0][2] , inv );
	u.tmpr8[0] = gf_mul( mat3[0][0] , col[1] );
	u.tmpr8[1] = gf_mul( mat3[0][1] , col[1] );
	u.tmpr8[2] = gf_mul( mat3[0][2] , col[1] );
	r1[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat3[0][0] , col[2] );
	u.tmpr8[1] = gf_mul( mat3[0][1] , col[2] );
	u.tmpr8[2] = gf_mul( mat3[0][2] , col[2] );
	r2[0] ^= u.tmpr;
/////////////
	col[0] = mat3[0][1];
	col[1] = mat3[1][1];
	col[2] = mat3[2][1];
	mat3[0][1] = 0;
	mat3[1][1] = 1;
	mat3[2][1] = 0;
	inv = gf_inv( col[1] );
	mat3[1][0] = gf_mul( mat3[1][0] , inv );
	mat3[1][1] = inv;
	mat3[1][2] = gf_mul( mat3[1][2] , inv );
	u.tmpr8[0] = gf_mul( mat3[1][0] , col[0] );
	u.tmpr8[1] = gf_mul( mat3[1][1] , col[0] );
	u.tmpr8[2] = gf_mul( mat3[1][2] , col[0] );
	r0[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat3[1][0] , col[2] );
	u.tmpr8[1] = gf_mul( mat3[1][1] , col[2] );
	u.tmpr8[2] = gf_mul( mat3[1][2] , col[2] );
	r2[0] ^= u.tmpr;
//////////////
	col[0] = mat3[0][2];
	col[1] = mat3[1][2];
	col[2] = mat3[2][2];
	mat3[0][2] = 0;
	mat3[1][2] = 0;
	mat3[2][2] = 1;
	inv = gf_inv( col[2] );
	mat3[2][0] = gf_mul( mat3[2][0] , inv );
	mat3[2][1] = gf_mul( mat3[2][1] , inv );
	mat3[2][2] = inv;
	u.tmpr8[0] = gf_mul( mat3[2][0] , col[0] );
	u.tmpr8[1] = gf_mul( mat3[2][1] , col[0] );
	u.tmpr8[2] = gf_mul( mat3[2][2] , col[0] );
	r0[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat3[2][0] , col[1] );
	u.tmpr8[1] = gf_mul( mat3[2][1] , col[1] );
	u.tmpr8[2] = gf_mul( mat3[2][2] , col[1] );
	r1[0] ^= u.tmpr;
}

inline void transpose_mat3( uint8_t *c0,uint8_t *c1,
		uint8_t *c2, uint8_t mat3[3][4] )
{
	c0[0] = mat3[0][0];
	c0[1] = mat3[1][0];
	c0[2] = mat3[2][0];

	c1[0] = mat3[0][1];
	c1[1] = mat3[1][1];
	c1[2] = mat3[2][1];

	c2[0] = mat3[0][2];
	c2[1] = mat3[1][2];
	c2[2] = mat3[2][2];
}

#if 1

void get_rec_coef3( uint8_t *rec_coe[] , int a, int b, int c, const uint8_t * coef[] )
{
//	uint8_t mat3[3][4];
	rec_coe[0][0] = coef[0][a];
	rec_coe[0][1] = coef[0][b];
	rec_coe[0][2] = coef[0][c];
	rec_coe[1][0] = coef[1][a];
	rec_coe[1][1] = coef[1][b];
	rec_coe[1][2] = coef[1][c];
	rec_coe[2][0] = coef[2][a];
	rec_coe[2][1] = coef[2][b];
	rec_coe[2][2] = coef[2][c];
	get_inv_mat3( rec_coe );
//	transpose_mat3( rec_coe[0] , rec_coe[1] ,rec_coe[2] , mat3 );
}


#else

void get_rec_coef3( uint8_t *rec_coe[] , int a, int b, int c, const uint8_t * coef[] )
{
	uint8_t mat3[3][4];
	mat3[0][0] = coef[0][a];
	mat3[1][0] = coef[0][b];
	mat3[2][0] = coef[0][c];
	mat3[0][1] = coef[1][a];
	mat3[1][1] = coef[1][b];
	mat3[2][1] = coef[1][c];
	mat3[0][2] = coef[2][a];
	mat3[1][2] = coef[2][b];
	mat3[2][2] = coef[2][c];
	get_inv_mat3( mat3 );
	transpose_mat3( rec_coe[0] , rec_coe[1] ,rec_coe[2] , mat3 );
}

#endif



void get_rec_coef3_xor( uint8_t *rec_coe[] , int a, int b, int c, const uint8_t * coef[] )
{
/*
	uint8_t mat3[3][4];
	mat3[0][0] = 1;
	mat3[1][0] = 1;
	mat3[2][0] = 1;
	mat3[0][1] = coef[1][a];
	mat3[1][1] = coef[1][b];
	mat3[2][1] = coef[1][c];
	mat3[0][2] = coef[2][a];
	mat3[1][2] = coef[2][b];
	mat3[2][2] = coef[2][c];
*/
	rec_coe[0][0] = 1;
	rec_coe[0][1] = 1;
	rec_coe[0][2] = 1;
	rec_coe[1][0] = coef[1][a];
	rec_coe[1][1] = coef[1][b];
	rec_coe[1][2] = coef[1][c];
	rec_coe[2][0] = coef[2][a];
	rec_coe[2][1] = coef[2][b];
	rec_coe[2][2] = coef[2][c];
	get_inv_mat3_xor( rec_coe );
//	transpose_mat3( rec_coe[0] , rec_coe[1] ,rec_coe[2] , mat3 );
}







///////////////////////////////////////





//static void get_inv_mat4( uint8_t mat[4][4] )
static void get_inv_mat4( uint8_t * mat[] )
{
	uint32_t * r0 = (uint32_t*)& mat[0][0];
	uint32_t * r1 = (uint32_t*)& mat[1][0];
	uint32_t * r2 = (uint32_t*)& mat[2][0];
	uint32_t * r3 = (uint32_t*)& mat[3][0];

	uint8_t col[4];
	uint8_t inv;
	union{
	uint8_t tmpr8[4];
	uint32_t tmpr;
	} u;

////////////

	col[0] = mat[0][0];
	col[1] = mat[1][0];
	col[2] = mat[2][0];
	col[3] = mat[3][0];
	mat[0][0] = 1;
	mat[1][0] = 0;
	mat[2][0] = 0;
	mat[3][0] = 0;
	inv = gf_inv( col[0] );
	mat[0][0] = inv;
	mat[0][1] = gf_mul( mat[0][1] , inv );
	mat[0][2] = gf_mul( mat[0][2] , inv );
	mat[0][3] = gf_mul( mat[0][3] , inv );

	u.tmpr8[0] = gf_mul( mat[0][0] , col[1] );
	u.tmpr8[1] = gf_mul( mat[0][1] , col[1] );
	u.tmpr8[2] = gf_mul( mat[0][2] , col[1] );
	u.tmpr8[3] = gf_mul( mat[0][3] , col[1] );
	r1[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat[0][0] , col[2] );
	u.tmpr8[1] = gf_mul( mat[0][1] , col[2] );
	u.tmpr8[2] = gf_mul( mat[0][2] , col[2] );
	u.tmpr8[3] = gf_mul( mat[0][3] , col[2] );
	r2[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat[0][0] , col[3] );
	u.tmpr8[1] = gf_mul( mat[0][1] , col[3] );
	u.tmpr8[2] = gf_mul( mat[0][2] , col[3] );
	u.tmpr8[3] = gf_mul( mat[0][3] , col[3] );
	r3[0] ^= u.tmpr;
/////////////

	col[0] = mat[0][1];
	col[1] = mat[1][1];
	col[2] = mat[2][1];
	col[3] = mat[3][1];
	mat[0][1] = 0;
	mat[1][1] = 1;
	mat[2][1] = 0;
	mat[3][1] = 0;
	inv = gf_inv( col[1] );
	mat[1][0] = gf_mul( mat[1][0] , inv );
	mat[1][1] = inv;
	mat[1][2] = gf_mul( mat[1][2] , inv );
	mat[1][3] = gf_mul( mat[1][3] , inv );

	u.tmpr8[0] = gf_mul( mat[1][0] , col[0] );
	u.tmpr8[1] = gf_mul( mat[1][1] , col[0] );
	u.tmpr8[2] = gf_mul( mat[1][2] , col[0] );
	u.tmpr8[3] = gf_mul( mat[1][3] , col[0] );
	 r0[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat[1][0] , col[2] );
	u.tmpr8[1] = gf_mul( mat[1][1] , col[2] );
	u.tmpr8[2] = gf_mul( mat[1][2] , col[2] );
	u.tmpr8[3] = gf_mul( mat[1][3] , col[2] );
	r2[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat[1][0] , col[3] );
	u.tmpr8[1] = gf_mul( mat[1][1] , col[3] );
	u.tmpr8[2] = gf_mul( mat[1][2] , col[3] );
	u.tmpr8[3] = gf_mul( mat[1][3] , col[3] );
	r3[0] ^= u.tmpr;

//////////////

	col[0] = mat[0][2];
	col[1] = mat[1][2];
	col[2] = mat[2][2];
	col[3] = mat[3][2];
	mat[0][2] = 0;
	mat[1][2] = 0;
	mat[2][2] = 1;
	mat[3][2] = 0;
	inv = gf_inv( col[2] );
	mat[2][0] = gf_mul( mat[2][0] , inv );
	mat[2][1] = gf_mul( mat[2][1] , inv );
	mat[2][2] = inv;
	mat[2][3] = gf_mul( mat[2][3] , inv );

	u.tmpr8[0] = gf_mul( mat[2][0] , col[0] );
	u.tmpr8[1] = gf_mul( mat[2][1] , col[0] );
	u.tmpr8[2] = gf_mul( mat[2][2] , col[0] );
	u.tmpr8[3] = gf_mul( mat[2][3] , col[0] );
	r0[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat[2][0] , col[1] );
	u.tmpr8[1] = gf_mul( mat[2][1] , col[1] );
	u.tmpr8[2] = gf_mul( mat[2][2] , col[1] );
	u.tmpr8[3] = gf_mul( mat[2][3] , col[1] );
	r1[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat[2][0] , col[3] );
	u.tmpr8[1] = gf_mul( mat[2][1] , col[3] );
	u.tmpr8[2] = gf_mul( mat[2][2] , col[3] );
	u.tmpr8[3] = gf_mul( mat[2][3] , col[3] );
	r3[0] ^= u.tmpr;

/////////////////////

	col[0] = mat[0][3];
	col[1] = mat[1][3];
	col[2] = mat[2][3];
	col[3] = mat[3][3];
	mat[0][3] = 0;
	mat[1][3] = 0;
	mat[2][3] = 0;
	mat[3][3] = 1;
	inv = gf_inv( col[3] );
	mat[3][0] = gf_mul( mat[3][0] , inv );
	mat[3][1] = gf_mul( mat[3][1] , inv );
	mat[3][2] = gf_mul( mat[3][2] , inv );
	mat[3][3] = inv;

	u.tmpr8[0] = gf_mul( mat[3][0] , col[0] );
	u.tmpr8[1] = gf_mul( mat[3][1] , col[0] );
	u.tmpr8[2] = gf_mul( mat[3][2] , col[0] );
	u.tmpr8[3] = gf_mul( mat[3][3] , col[0] );
	r0[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat[3][0] , col[1] );
	u.tmpr8[1] = gf_mul( mat[3][1] , col[1] );
	u.tmpr8[2] = gf_mul( mat[3][2] , col[1] );
	u.tmpr8[3] = gf_mul( mat[3][3] , col[1] );
	r1[0] ^= u.tmpr;
	u.tmpr8[0] = gf_mul( mat[3][0] , col[2] );
	u.tmpr8[1] = gf_mul( mat[3][1] , col[2] );
	u.tmpr8[2] = gf_mul( mat[3][2] , col[2] );
	u.tmpr8[3] = gf_mul( mat[3][3] , col[2] );
	r2[0] ^= u.tmpr;

}

inline void transpose_mat4( uint8_t *c0,uint8_t *c1,
		uint8_t *c2, uint8_t *c3, uint8_t mat[4][4] )
{
	c0[0] = mat[0][0];
	c0[1] = mat[1][0];
	c0[2] = mat[2][0];
	c0[3] = mat[3][0];

	c1[0] = mat[0][1];
	c1[1] = mat[1][1];
	c1[2] = mat[2][1];
	c1[3] = mat[3][1];

	c2[0] = mat[0][2];
	c2[1] = mat[1][2];
	c2[2] = mat[2][2];
	c2[3] = mat[3][2];

	c3[0] = mat[0][3];
	c3[1] = mat[1][3];
	c3[2] = mat[2][3];
	c3[3] = mat[3][3];
}



#if 1

void get_rec_coef4( uint8_t * rec_coe[] , uint8_t idx[], const uint8_t * coef[] )
{
//	uint8_t mat[4][4];
	rec_coe[0][0] = coef[0][idx[0]];
	rec_coe[0][1] = coef[0][idx[1]];
	rec_coe[0][2] = coef[0][idx[2]];
	rec_coe[0][3] = coef[0][idx[3]];

	rec_coe[1][0] = coef[1][idx[0]];
	rec_coe[1][1] = coef[1][idx[1]];
	rec_coe[1][2] = coef[1][idx[2]];
	rec_coe[1][3] = coef[1][idx[3]];

	rec_coe[2][0] = coef[2][idx[0]];
	rec_coe[2][1] = coef[2][idx[1]];
	rec_coe[2][2] = coef[2][idx[2]];
	rec_coe[2][3] = coef[2][idx[3]];

	rec_coe[3][0] = coef[3][idx[0]];
	rec_coe[3][1] = coef[3][idx[1]];
	rec_coe[3][2] = coef[3][idx[2]];
	rec_coe[3][3] = coef[3][idx[3]];

	get_inv_mat4( rec_coe );
//	transpose_mat4( rec_coe[0] , rec_coe[1] ,rec_coe[2] ,rec_coe[3] , mat );
}

#else


void get_rec_coef4( uint8_t *rec_coe[] , uint8_t idx[], const uint8_t * coef[] )
{
	uint8_t mat[4][4];
	mat[0][0] = coef[0][idx[0]];
	mat[1][0] = coef[0][idx[1]];
	mat[2][0] = coef[0][idx[2]];
	mat[3][0] = coef[0][idx[3]];

	mat[0][1] = coef[1][idx[0]];
	mat[1][1] = coef[1][idx[1]];
	mat[2][1] = coef[1][idx[2]];
	mat[3][1] = coef[1][idx[3]];

	mat[0][2] = coef[2][idx[0]];
	mat[1][2] = coef[2][idx[1]];
	mat[2][2] = coef[2][idx[2]];
	mat[3][2] = coef[2][idx[3]];

	mat[0][3] = coef[3][idx[0]];
	mat[1][3] = coef[3][idx[1]];
	mat[2][3] = coef[3][idx[2]];
	mat[3][3] = coef[3][idx[3]];

	get_inv_mat4( mat );
	transpose_mat4( rec_coe[0] , rec_coe[1] ,rec_coe[2] ,rec_coe[3] , mat );
}

#endif


#include "gf65536.h"


static void get_inv_mat4_xor_gf216( uint16_t * mat[] )
{
	uint64_t * r0 = (uint64_t*) mat[0];
	uint64_t * r1 = (uint64_t*) mat[1];
	uint64_t * r2 = (uint64_t*) mat[2];
	uint64_t * r3 = (uint64_t*) mat[3];

	uint16_t col[4];

	uint8_t inv;
	uint16_t inv16;
	union{
	uint16_t tmpr16[4];
	uint64_t tmpr;
	} u;

////////////

	col[0] = 1;
	col[1] = mat[1][0];
	col[2] = mat[2][0];
	col[3] = mat[3][0];
	//mat[0][0] = 1;
	mat[1][0] = 0;
	mat[2][0] = 0;
	mat[3][0] = 0;
	//inv = gf_inv( col[0] ); /// inv = 1;
	// mat[0][0] = inv;
	//mat[0][1] = gf_mul( mat[0][1] , inv );
	//mat[0][2] = gf_mul( mat[0][2] , inv );
	//mat[0][3] = gf_mul( mat[0][3] , inv );

	u.tmpr16[0] = col[1]; // = gf_mul( mat[0][0] , col[1] );
	u.tmpr16[1] = col[1]; // = gf_mul( mat[0][1] , col[1] );
	u.tmpr16[2] = col[1]; // = gf_mul( mat[0][2] , col[1] );
	u.tmpr16[3] = col[1]; // = gf_mul( mat[0][3] , col[1] );
	r1[0] ^= u.tmpr;
	u.tmpr16[0] = col[2]; // = gf_mul( mat[0][0] , col[2] );
	u.tmpr16[1] = col[2]; // = gf_mul( mat[0][1] , col[2] );
	u.tmpr16[2] = col[2]; // = gf_mul( mat[0][2] , col[2] );
	u.tmpr16[3] = col[2]; // = gf_mul( mat[0][3] , col[2] );
	r2[0] ^= u.tmpr;
	u.tmpr16[0] = col[3]; // = gf_mul( mat[0][0] , col[3] );
	u.tmpr16[1] = col[3]; // = gf_mul( mat[0][1] , col[3] );
	u.tmpr16[2] = col[3]; // = gf_mul( mat[0][2] , col[3] );
	u.tmpr16[3] = col[3]; // = gf_mul( mat[0][3] , col[3] );
	r3[0] ^= u.tmpr;

/////////////

	col[0] = 1; /// mat[0][1];
	col[1] = mat[1][1];
	col[2] = mat[2][1];
	col[3] = mat[3][1];
	mat[0][1] = 0;
	mat[1][1] = 1;
	mat[2][1] = 0;
	mat[3][1] = 0;
	inv = gf_inv( (uint8_t)col[1] );
	mat[1][0] = gf_mul( mat[1][0] , inv );
	mat[1][1] = inv;
	mat[1][2] = gf_mul( mat[1][2] , inv );
	mat[1][3] = gf_mul( mat[1][3] , inv );

	u.tmpr16[0] = mat[1][0]; /// gf_mul( mat[1][0] , col[0] );
	u.tmpr16[1] = mat[1][1]; /// gf_mul( mat[1][1] , col[0] );
	u.tmpr16[2] = mat[1][2]; /// gf_mul( mat[1][2] , col[0] );
	u.tmpr16[3] = mat[1][3]; /// gf_mul( mat[1][3] , col[0] );
	r0[0] ^= u.tmpr;
	u.tmpr16[0] = gf_mul( mat[1][0] , col[2] );
	u.tmpr16[1] = gf_mul( mat[1][1] , col[2] );
	u.tmpr16[2] = gf_mul( mat[1][2] , col[2] );
	u.tmpr16[3] = gf_mul( mat[1][3] , col[2] );
	r2[0] ^= u.tmpr;
	u.tmpr16[0] = gf216_mul_gf256( col[3] , mat[1][0] );
	u.tmpr16[1] = gf216_mul_gf256( col[3] , mat[1][1] );
	u.tmpr16[2] = gf216_mul_gf256( col[3] , mat[1][2] );
	u.tmpr16[3] = gf216_mul_gf256( col[3] , mat[1][3] );
	r3[0] ^= u.tmpr;

//////////////

	col[0] = mat[0][2];
	col[1] = mat[1][2];
	col[2] = mat[2][2];
	col[3] = mat[3][2];
	mat[0][2] = 0;
	mat[1][2] = 0;
	mat[2][2] = 1;
	mat[3][2] = 0;
	inv = gf_inv( col[2] );
	mat[2][0] = gf_mul( mat[2][0] , inv );
	mat[2][1] = gf_mul( mat[2][1] , inv );
	mat[2][2] = inv;
	mat[2][3] = gf_mul( mat[2][3] , inv );

	u.tmpr16[0] = gf_mul( mat[2][0] , col[0] );
	u.tmpr16[1] = gf_mul( mat[2][1] , col[0] );
	u.tmpr16[2] = gf_mul( mat[2][2] , col[0] );
	u.tmpr16[3] = gf_mul( mat[2][3] , col[0] );
	r0[0] ^= u.tmpr;
	u.tmpr16[0] = gf_mul( mat[2][0] , col[1] );
	u.tmpr16[1] = gf_mul( mat[2][1] , col[1] );
	u.tmpr16[2] = gf_mul( mat[2][2] , col[1] );
	u.tmpr16[3] = gf_mul( mat[2][3] , col[1] );
	r1[0] ^= u.tmpr;
	u.tmpr16[0] = gf216_mul_gf256( col[3] , mat[2][0] );
	u.tmpr16[1] = gf216_mul_gf256( col[3] , mat[2][1] );
	u.tmpr16[2] = gf216_mul_gf256( col[3] , mat[2][2] );
	u.tmpr16[3] = gf216_mul_gf256( col[3] , mat[2][3] );
	r3[0] ^= u.tmpr;

/////////////////////

	col[0] = mat[0][3];
	col[1] = mat[1][3];
	col[2] = mat[2][3];
	col[3] = mat[3][3];
	mat[0][3] = 0;
	mat[1][3] = 0;
	mat[2][3] = 0;
	mat[3][3] = 1;
	inv16 = gf216_inv( col[3] );
	mat[3][0] = gf216_mul( mat[3][0] , inv16 );
	mat[3][1] = gf216_mul( mat[3][1] , inv16 );
	mat[3][2] = gf216_mul( mat[3][2] , inv16 );
	mat[3][3] = inv16;

	u.tmpr16[0] = gf216_mul_gf256( mat[3][0] , col[0] );
	u.tmpr16[1] = gf216_mul_gf256( mat[3][1] , col[0] );
	u.tmpr16[2] = gf216_mul_gf256( mat[3][2] , col[0] );
	u.tmpr16[3] = gf216_mul_gf256( mat[3][3] , col[0] );
	r0[0] ^= u.tmpr;
	u.tmpr16[0] = gf216_mul_gf256( mat[3][0] , col[1] );
	u.tmpr16[1] = gf216_mul_gf256( mat[3][1] , col[1] );
	u.tmpr16[2] = gf216_mul_gf256( mat[3][2] , col[1] );
	u.tmpr16[3] = gf216_mul_gf256( mat[3][3] , col[1] );
	r1[0] ^= u.tmpr;
	u.tmpr16[0] = gf216_mul_gf256( mat[3][0] , col[2] );
	u.tmpr16[1] = gf216_mul_gf256( mat[3][1] , col[2] );
	u.tmpr16[2] = gf216_mul_gf256( mat[3][2] , col[2] );
	u.tmpr16[3] = gf216_mul_gf256( mat[3][3] , col[2] );
	r2[0] ^= u.tmpr;
}



void get_rec_coef4_xor_gf216( uint16_t *rec_coe[] , uint8_t idx[], const uint8_t * coef[] , const uint16_t coef216[] )
{
	rec_coe[0][0] = coef[0][idx[0]];
	rec_coe[0][1] = coef[0][idx[1]];
	rec_coe[0][2] = coef[0][idx[2]];
	rec_coe[0][3] = coef[0][idx[3]];

	rec_coe[1][0] = coef[1][idx[0]];
	rec_coe[1][1] = coef[1][idx[1]];
	rec_coe[1][2] = coef[1][idx[2]];
	rec_coe[1][3] = coef[1][idx[3]];

	rec_coe[2][0] = coef[2][idx[0]];
	rec_coe[2][1] = coef[2][idx[1]];
	rec_coe[2][2] = coef[2][idx[2]];
	rec_coe[2][3] = coef[2][idx[3]];

	rec_coe[3][0] = coef216[idx[0]];
	rec_coe[3][1] = coef216[idx[1]];
	rec_coe[3][2] = coef216[idx[2]];
	rec_coe[3][3] = coef216[idx[3]];

	get_inv_mat4_xor_gf216( rec_coe );
}

