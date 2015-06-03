#ifndef _RAIDQ_HPP_
#define _RAIDQ_HPP_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "raid_table.h"
#include "gf256.h"
#include "gf256_xmm.h"
#include "gf65536.h"

#include "recover.h"

#include "recover.hpp"

//#define _USE_ASM_


template <typename sto_t>
void raid5_gen_syndrome(int disks, size_t bytes, void **ptrs )
{
	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - 2;       /// Highest data disk

	sto_t *ps = sto_ptrs[z0+1];

	sto_t ck_sum;
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		ck_sum = sto_ptrs[z0][i];
		for( int z = z0-1; z >= 0 ; z-- ) {
			ck_sum ^= sto_ptrs[z][i];
		}
		ps[i] = ck_sum;
	}
}

template <typename sto_t>
void raid5_recov(int disks, size_t bytes, int faila, void **ptrs )
{
	void * tmp = ptrs[faila];
	ptrs[faila]=ptrs[disks-1];
	ptrs[disks-1]=tmp;
	raid5_gen_syndrome<sto_t>(disks,bytes,ptrs);
	tmp = ptrs[faila];
	ptrs[faila]=ptrs[disks-1];
	ptrs[disks-1]=tmp;
}


//////////////////////////////////////


/// not even a code
template <typename sto_t>
void naive6_gen_syndrome(int disks, size_t bytes, void **ptrs )
{
	const unsigned P = 2;

	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for(unsigned j=0;j<P;j++) ps[j]=sto_ptrs[z0+1+j];

	sto_t ck_sum[P];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		for( int z = z0-1; z >= 0 ; z-- ) {
			ck_sum[0] ^= sto_ptrs[z][i];
			ck_sum[1] &= sto_ptrs[z][i];
		}
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}

/////////////////////////////////////////////

template <typename sto_t>
void raid6_gen_syndrome(int disks, size_t bytes, void **ptrs )
{
	const unsigned P = 2;

	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for(unsigned j=0;j<P;j++) ps[j]=sto_ptrs[z0+1+j];

	sto_t ck_sum[P];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		for( int z = z0-1; z >= 0 ; z-- ) {
			mul_alpha(ck_sum[1]);
			for(unsigned j=0;j<P;j++) ck_sum[j] ^= sto_ptrs[z][i];
		}
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}

////////////////////////////////////////////////////////////////


/// CAUTION: fail_idx[i] < datas  and ascending ordering.
template <typename sto_t>
void raid6_recov_data( int datas, size_t bytes, uint8_t fail_idx[], void ** void_ptrs )
{
        const unsigned P = 2;

        uint8_t coe0[2];
        uint8_t coe1[2];
        uint8_t * rec_coef[] = { coe0, coe1 };
        get_rec_coef2_xor( rec_coef, fail_idx[0] , fail_idx[1] , raid_sqrt );

        sto_t ** sto_ptrs = (sto_t**) void_ptrs;
        sto_t * fail_ptrs[P];
        for(unsigned i=0;i<P;i++) {
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
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		for( int z = z0-1; z >= 0 ; z-- ) {
			mul_alpha(ck_sum[1]);
			for(unsigned j=0;j<P;j++) ck_sum[j] ^= sto_ptrs[z][i];
		}
		for(unsigned j=0;j<P;j++) ck_sum[j] ^= ps[j][i];
		for(unsigned j=0;j<P;j++) {
			res = gfv_mul( ck_sum[0] , rec_coef[j][0] );
			for( unsigned k=1;k<P;k++) res ^= gfv_mul( ck_sum[k] , rec_coef[j][k] );
			fail_ptrs[j][i] = res;
		}
	}
	for(unsigned i=0;i<P;i++) {
		sto_ptrs[fail_idx[i]] = fail_ptrs[i];
	}
}



//////////////////////////////////////////////////


/// not even a code
template <typename sto_t>
void naive7_gen_syndrome(int disks, size_t bytes, void **ptrs )
{
	const unsigned P = 3;

	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for(unsigned j=0;j<P;j++) ps[j]=sto_ptrs[z0+1+j];

	sto_t ck_sum[P];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		for( int z = z0-1; z >= 0 ; z-- ) {
			ck_sum[0] ^= sto_ptrs[z][i];
			ck_sum[1] &= sto_ptrs[z][i];
			ck_sum[2] |= sto_ptrs[z][i];
		}
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}


////////////////////////////////////////////////////////////////////



template <typename sto_t>
void raidq7_a2_gen_syndrome(int disks, size_t bytes, void **ptrs )
{
	const unsigned P = 3;

	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for(unsigned j=0;j<P;j++) ps[j]=sto_ptrs[z0+1+j];

	sto_t ck_sum[P];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		for( int z = z0-1; z >= 0 ; z-- ) {
			mul_alpha(ck_sum[1]);
			mul_alpha2(ck_sum[2]);
			for(unsigned j=0;j<P;j++) ck_sum[j] ^= sto_ptrs[z][i];
		}
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}

template <typename sto_t>
void raidq7_a2_recov_data( int datas, size_t bytes, uint8_t fail_idx[], void ** void_ptrs )
{
	const unsigned P = 3;

	uint8_t coe0[4];
	uint8_t coe1[4];
	uint8_t coe2[4];
	uint8_t * rec_coef[] = { coe0, coe1 , coe2 };
	get_rec_coef3_xor( rec_coef, fail_idx[0] , fail_idx[1] , fail_idx[2] , raid_a2 );

        sto_t ** sto_ptrs = (sto_t**) void_ptrs;
        sto_t * fail_ptrs[P];
        for(unsigned i=0;i<P;i++) {
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
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		for( int z = z0-1; z >= 0 ; z-- ) {
			mul_alpha(ck_sum[1]);
			mul_alpha2(ck_sum[2]);
			for(unsigned j=0;j<P;j++) ck_sum[j] ^= sto_ptrs[z][i];
		}

		for(unsigned j=0;j<P;j++) ck_sum[j] ^= ps[j][i];
		for(unsigned j=0;j<P;j++) {
			res = gfv_mul( ck_sum[0] , rec_coef[j][0] );
			for( unsigned k=1;k<P;k++) res ^= gfv_mul( ck_sum[k] , rec_coef[j][k] );
			fail_ptrs[j][i] = res;
		}
	}
	for(unsigned i=0;i<P;i++) {
		sto_ptrs[fail_idx[i]] = fail_ptrs[i];
	}
}




//////////////////////////////////////////////////


template <typename sto_t>
void raidq7_sqrta_gen_syndrome(int disks, size_t bytes, void **ptrs )
{
	const unsigned P = 3;

	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for(unsigned j=0;j<P;j++) ps[j]=sto_ptrs[z0+1+j];

	sto_t ck_sum[P];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		for( int z = z0-1; z >= 0 ; z-- ) {
			mul_alpha(ck_sum[1]);
			mul_sqrt_alpha(ck_sum[2]);
			for(unsigned j=0;j<P;j++) ck_sum[j] ^= sto_ptrs[z][i];
		}
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}

///////////////////////////////////////////////////

/// data-disks should be multiple of 2
template <typename sto_t>
void raidq7_sqrta_gen_syndrome2(int disks, size_t bytes, void **ptrs )
{
	assert(0==(1&(disks-3)));
	const unsigned P = 3;

	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for(unsigned j=0;j<P;j++) ps[j]=sto_ptrs[z0+1+j];

	sto_t ck_sum[P+1];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		mul_alpha(ck_sum[1]);
		ck_sum[0]^= sto_ptrs[z0-1][i];
		ck_sum[1]^= sto_ptrs[z0-1][i];
		ck_sum[3]= sto_ptrs[z0-1][i];

		for( int z = z0-2; z >= 0 ; z-=2 ) {
			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[2]);
			ck_sum[0]^=sto_ptrs[z][i];
			ck_sum[1]^=sto_ptrs[z][i];
			ck_sum[2]^=sto_ptrs[z][i];

			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z-1][i];
			ck_sum[1]^=sto_ptrs[z-1][i];
			ck_sum[3]^=sto_ptrs[z-1][i];
		}
		mul_sqrt_alpha(ck_sum[2]);
		ck_sum[2]^=ck_sum[3];
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}

/////////////////////////////////////////////////


/// CAUTION: fail_idx[i] < datas  and ascending ordering.
template <typename sto_t>
void raidq7_sqrta_recov_data2( int datas, size_t bytes, uint8_t fail_idx[], void ** void_ptrs )
{
	assert(0==(1&datas));
	const unsigned P = 3;

	uint8_t coe0[4];
	uint8_t coe1[4];
	uint8_t coe2[4];
	uint8_t * rec_coef[] = { coe0, coe1 , coe2 };
	get_rec_coef3_xor( rec_coef, fail_idx[0] , fail_idx[1] , fail_idx[2] , raid_sqrt );
	//get_rec_coef3( rec_coef, fail_idx[0] , fail_idx[1] , fail_idx[2] , raid_sqrt );

        sto_t ** sto_ptrs = (sto_t**) void_ptrs;
        sto_t * fail_ptrs[P];
        for(unsigned i=0;i<P;i++) {
                fail_ptrs[i] = sto_ptrs[fail_idx[i]];
                sto_ptrs[fail_idx[i]] = (sto_t*)raid6_empty_zero_page;
        }
        sto_t *ps[P];
        for( unsigned i=0;i<P;i++) ps[i] = sto_ptrs[datas+i];

        int z0 = datas - 1;
	sto_t ck_sum[P+1];
	sto_t res;
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		mul_alpha(ck_sum[1]);
		ck_sum[0]^= sto_ptrs[z0-1][i];
		ck_sum[1]^= sto_ptrs[z0-1][i];
		ck_sum[3]= sto_ptrs[z0-1][i];

		for( int z = z0-2; z >= 0 ; z-=2 ) {
			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[2]);
			ck_sum[0]^=sto_ptrs[z][i];
			ck_sum[1]^=sto_ptrs[z][i];
			ck_sum[2]^=sto_ptrs[z][i];

			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z-1][i];
			ck_sum[1]^=sto_ptrs[z-1][i];
			ck_sum[3]^=sto_ptrs[z-1][i];
		}
		mul_sqrt_alpha(ck_sum[2]);
		ck_sum[2]^=ck_sum[3];
		for(unsigned j=0;j<P;j++) ck_sum[j] ^= ps[j][i];
		for(unsigned j=0;j<P;j++) {
			res = gfv_mul( ck_sum[0] , rec_coef[j][0] );
			for( unsigned k=1;k<P;k++) res ^= gfv_mul( ck_sum[k] , rec_coef[j][k] );
			fail_ptrs[j][i] = res;
		}
	}
	for(unsigned i=0;i<P;i++) {
		sto_ptrs[fail_idx[i]] = fail_ptrs[i];
	}
}



//////////////////////////////////////////////////


/// not even a code
template <typename sto_t>
void naive8_gen_syndrome(int disks, size_t bytes, void **ptrs )
{
	const unsigned P = 4;

	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for(unsigned j=0;j<P;j++) ps[j]=sto_ptrs[z0+1+j];

	sto_t ck_sum[P];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		for( int z = z0-1; z >= 0 ; z-- ) {
			ck_sum[0] ^= sto_ptrs[z][i];
			ck_sum[1] &= sto_ptrs[z][i];
			ck_sum[2] |= sto_ptrs[z][i];
			and_not( ck_sum[3] , sto_ptrs[z][i] );
		}
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}


/// disks should be multiple of 2
template <typename sto_t>
void raidq8_X_gen_syndrome(int disks, size_t bytes, void **ptrs )
{
	const unsigned P = 4;

	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for(unsigned j=0;j<P;j++) ps[j]=sto_ptrs[z0+1+j];

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
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}

/// CAUTION: fail_idx[i] < datas  and ascending ordering.
/// disks should be multiple of 2
template <typename sto_t>
void raidq8_X_recov_data( int datas, size_t bytes, uint8_t fail_idx[], void ** void_ptrs )
{
	assert(0==(1&datas));
	const unsigned P = 4;

	uint16_t coe0[4];
	uint16_t coe1[4];
	uint16_t coe2[4];
	uint16_t coe3[4];
	uint16_t * rec_coef[] = { coe0, coe1 , coe2 , coe3 };
	get_rec_coef4_xor_gf216( rec_coef, fail_idx , raid_sqrt , raidq8_X );

        sto_t ** sto_ptrs = (sto_t**) void_ptrs;
        sto_t * fail_ptrs[P];
        for(unsigned i=0;i<P;i++) {
                fail_ptrs[i] = sto_ptrs[fail_idx[i]];
                sto_ptrs[fail_idx[i]] = (sto_t*)raid6_empty_zero_page;
        }
        sto_t *ps[P];
        for( unsigned i=0;i<P;i++) ps[i] = sto_ptrs[datas+i];

        int z0 = datas - 1;
	sto_t ck_sum[P+1];
	sto_t res;
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
		for(unsigned j=0;j<P;j++) ck_sum[j] ^= ps[j][i];
		for(unsigned j=0;j<P;j++) {
			res = gf216bsv_mul( ck_sum[0] , rec_coef[j][0] );
			for( unsigned k=1;k<P;k++) res ^= gf216bsv_mul( ck_sum[k] , rec_coef[j][k] );
			fail_ptrs[j][i] = res;
		}
	}
	for(unsigned i=0;i<P;i++) {
		sto_ptrs[fail_idx[i]] = fail_ptrs[i];
	}
}





/// disks should be multiple of 2
template <typename sto_t>
void raidq8_a2Xp1_gen_syndrome(int disks, size_t bytes, void **ptrs )
{
	const unsigned P = 4;

	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for(unsigned j=0;j<P;j++) ps[j]=sto_ptrs[z0+1+j];

	sto_t ck_sum[P+1];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		mul_alpha(ck_sum[1]);
		gf216bsv_mul_0x0401(ck_sum[3]);
		ck_sum[0]^= sto_ptrs[z0-1][i];
		ck_sum[1]^= sto_ptrs[z0-1][i];
		ck_sum[4] = sto_ptrs[z0-1][i];
		ck_sum[3]^= sto_ptrs[z0-1][i];

		for( int z = z0-2; z >= 0 ; z-=2 ) {
			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[2]);
			gf216bsv_mul_0x0401(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z][i];
			ck_sum[1]^=sto_ptrs[z][i];
			ck_sum[2]^=sto_ptrs[z][i];
			ck_sum[3]^=sto_ptrs[z][i];

			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[4]);
			gf216bsv_mul_0x0401(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z-1][i];
			ck_sum[1]^=sto_ptrs[z-1][i];
			ck_sum[3]^=sto_ptrs[z-1][i];
			ck_sum[4]^=sto_ptrs[z-1][i];
		}
		mul_sqrt_alpha(ck_sum[2]);
		ck_sum[2]^=ck_sum[4];
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}


/// CAUTION: fail_idx[i] < datas  and ascending ordering.
/// disks should be multiple of 2
template <typename sto_t>
void raidq8_a2Xp1_recov_data( int datas, size_t bytes, uint8_t fail_idx[], void ** void_ptrs )
{
	assert(0==(1&datas));
	const unsigned P = 4;

	uint16_t coe0[4];
	uint16_t coe1[4];
	uint16_t coe2[4];
	uint16_t coe3[4];
	uint16_t * rec_coef[] = { coe0, coe1 , coe2 , coe3 };
	get_rec_coef4_xor_gf216( rec_coef, fail_idx , raid_sqrt , raidq8_a2Xp1 );

        sto_t ** sto_ptrs = (sto_t**) void_ptrs;
        sto_t * fail_ptrs[P];
        for(unsigned i=0;i<P;i++) {
                fail_ptrs[i] = sto_ptrs[fail_idx[i]];
                sto_ptrs[fail_idx[i]] = (sto_t*)raid6_empty_zero_page;
        }
        sto_t *ps[P];
        for( unsigned i=0;i<P;i++) ps[i] = sto_ptrs[datas+i];

        int z0 = datas - 1;
	sto_t ck_sum[P+1];
	sto_t res;
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		mul_alpha(ck_sum[1]);
		gf216bsv_mul_0x0401(ck_sum[3]);
		ck_sum[0]^= sto_ptrs[z0-1][i];
		ck_sum[1]^= sto_ptrs[z0-1][i];
		ck_sum[4] = sto_ptrs[z0-1][i];
		ck_sum[3]^= sto_ptrs[z0-1][i];

		for( int z = z0-2; z >= 0 ; z-=2 ) {
			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[2]);
			gf216bsv_mul_0x0401(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z][i];
			ck_sum[1]^=sto_ptrs[z][i];
			ck_sum[2]^=sto_ptrs[z][i];
			ck_sum[3]^=sto_ptrs[z][i];

			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[4]);
			gf216bsv_mul_0x0401(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z-1][i];
			ck_sum[1]^=sto_ptrs[z-1][i];
			ck_sum[3]^=sto_ptrs[z-1][i];
			ck_sum[4]^=sto_ptrs[z-1][i];
		}
		mul_sqrt_alpha(ck_sum[2]);
		ck_sum[2]^=ck_sum[4];
		for(unsigned j=0;j<P;j++) ck_sum[j] ^= ps[j][i];
		for(unsigned j=0;j<P;j++) {
			res = gf216bsv_mul( ck_sum[0] , rec_coef[j][0] );
			for( unsigned k=1;k<P;k++) res ^= gf216bsv_mul( ck_sum[k] , rec_coef[j][k] );
			fail_ptrs[j][i] = res;
		}
	}
	for(unsigned i=0;i<P;i++) {
		sto_ptrs[fail_idx[i]] = fail_ptrs[i];
	}
}




/// disks should be multiple of 2
template <typename sto_t>
void raidq8_a141X_gen_syndrome(int disks, size_t bytes, void **ptrs )
{
	const unsigned P = 4;

	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for(unsigned j=0;j<P;j++) ps[j]=sto_ptrs[z0+1+j];

	sto_t ck_sum[P+1];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		mul_alpha(ck_sum[1]);
		gf216bsv_mul_0x1500(ck_sum[3]);
		ck_sum[0]^= sto_ptrs[z0-1][i];
		ck_sum[1]^= sto_ptrs[z0-1][i];
		ck_sum[4] = sto_ptrs[z0-1][i];
		ck_sum[3]^= sto_ptrs[z0-1][i];

		for( int z = z0-2; z >= 0 ; z-=2 ) {
			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[2]);
			gf216bsv_mul_0x1500(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z][i];
			ck_sum[1]^=sto_ptrs[z][i];
			ck_sum[2]^=sto_ptrs[z][i];
			ck_sum[3]^=sto_ptrs[z][i];

			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[4]);
			gf216bsv_mul_0x1500(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z-1][i];
			ck_sum[1]^=sto_ptrs[z-1][i];
			ck_sum[3]^=sto_ptrs[z-1][i];
			ck_sum[4]^=sto_ptrs[z-1][i];
		}
		mul_sqrt_alpha(ck_sum[2]);
		ck_sum[2]^=ck_sum[4];
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}


/// disks should be multiple of 2
template <typename sto_t>
void raidq8_a141X_recov_data( int datas, size_t bytes, uint8_t fail_idx[], void ** void_ptrs )
{
	assert(0==(1&datas));
	const unsigned P = 4;

	uint16_t coe0[4];
	uint16_t coe1[4];
	uint16_t coe2[4];
	uint16_t coe3[4];
	uint16_t * rec_coef[] = { coe0, coe1 , coe2 , coe3 };
	get_rec_coef4_xor_gf216( rec_coef, fail_idx , raid_sqrt , raidq8_a141X );

        sto_t ** sto_ptrs = (sto_t**) void_ptrs;
        sto_t * fail_ptrs[P];
        for(unsigned i=0;i<P;i++) {
                fail_ptrs[i] = sto_ptrs[fail_idx[i]];
                sto_ptrs[fail_idx[i]] = (sto_t*)raid6_empty_zero_page;
        }
        sto_t *ps[P];
        for( unsigned i=0;i<P;i++) ps[i] = sto_ptrs[datas+i];

        int z0 = datas - 1;
	sto_t ck_sum[P+1];
	sto_t res;
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		mul_alpha(ck_sum[1]);
		gf216bsv_mul_0x1500(ck_sum[3]);
		ck_sum[0]^= sto_ptrs[z0-1][i];
		ck_sum[1]^= sto_ptrs[z0-1][i];
		ck_sum[4] = sto_ptrs[z0-1][i];
		ck_sum[3]^= sto_ptrs[z0-1][i];

		for( int z = z0-2; z >= 0 ; z-=2 ) {
			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[2]);
			gf216bsv_mul_0x1500(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z][i];
			ck_sum[1]^=sto_ptrs[z][i];
			ck_sum[2]^=sto_ptrs[z][i];
			ck_sum[3]^=sto_ptrs[z][i];

			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[4]);
			gf216bsv_mul_0x1500(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z-1][i];
			ck_sum[1]^=sto_ptrs[z-1][i];
			ck_sum[3]^=sto_ptrs[z-1][i];
			ck_sum[4]^=sto_ptrs[z-1][i];
		}
		mul_sqrt_alpha(ck_sum[2]);
		ck_sum[2]^=ck_sum[4];
		for(unsigned j=0;j<P;j++) ck_sum[j] ^= ps[j][i];
		for(unsigned j=0;j<P;j++) {
			res = gf216bsv_mul( ck_sum[0] , rec_coef[j][0] );
			for( unsigned k=1;k<P;k++) res ^= gf216bsv_mul( ck_sum[k] , rec_coef[j][k] );
			fail_ptrs[j][i] = res;
		}
	}
	for(unsigned i=0;i<P;i++) {
		sto_ptrs[fail_idx[i]] = fail_ptrs[i];
	}
}




/// disks should be multiple of 2
template <typename sto_t>
void raidq8_a186Xpa6_gen_syndrome(int disks, size_t bytes, void **ptrs )
{
	const unsigned P = 4;

	sto_t **sto_ptrs = (sto_t **)ptrs;
	int z0 = disks - (P+1);       /// Highest data disk

	sto_t *ps[P];
	for(unsigned j=0;j<P;j++) ps[j]=sto_ptrs[z0+1+j];

	sto_t ck_sum[P+1];
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		mul_alpha(ck_sum[1]);
		gf216bsv_mul_0x6e40(ck_sum[3]);
		ck_sum[0]^= sto_ptrs[z0-1][i];
		ck_sum[1]^= sto_ptrs[z0-1][i];
		ck_sum[4] = sto_ptrs[z0-1][i];
		ck_sum[3]^= sto_ptrs[z0-1][i];

		for( int z = z0-2; z >= 0 ; z-=2 ) {
			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[2]);
			gf216bsv_mul_0x6e40(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z][i];
			ck_sum[1]^=sto_ptrs[z][i];
			ck_sum[2]^=sto_ptrs[z][i];
			ck_sum[3]^=sto_ptrs[z][i];

			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[4]);
			gf216bsv_mul_0x6e40(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z-1][i];
			ck_sum[1]^=sto_ptrs[z-1][i];
			ck_sum[3]^=sto_ptrs[z-1][i];
			ck_sum[4]^=sto_ptrs[z-1][i];
		}
		mul_sqrt_alpha(ck_sum[2]);
		ck_sum[2]^=ck_sum[4];
		for(unsigned j=0;j<P;j++) ps[j][i] = ck_sum[j];
	}
}



/// disks should be multiple of 2
template <typename sto_t>
void raidq8_a186Xpa6_recov_data( int datas, size_t bytes, uint8_t fail_idx[], void ** void_ptrs )
{
	assert(0==(1&datas));
	const unsigned P = 4;

	uint16_t coe0[4];
	uint16_t coe1[4];
	uint16_t coe2[4];
	uint16_t coe3[4];
	uint16_t * rec_coef[] = { coe0, coe1 , coe2 , coe3 };
	get_rec_coef4_xor_gf216( rec_coef, fail_idx , raid_sqrt , raidq8_a186Xpa6 );

        sto_t ** sto_ptrs = (sto_t**) void_ptrs;
        sto_t * fail_ptrs[P];
        for(unsigned i=0;i<P;i++) {
                fail_ptrs[i] = sto_ptrs[fail_idx[i]];
                sto_ptrs[fail_idx[i]] = (sto_t*)raid6_empty_zero_page;
        }
        sto_t *ps[P];
        for( unsigned i=0;i<P;i++) ps[i] = sto_ptrs[datas+i];

        int z0 = datas - 1;
	sto_t ck_sum[P+1];
	sto_t res;
	unsigned times = bytes/sizeof(sto_t);
	for( unsigned i = 0 ; i < times ; i ++ ) {
		for(unsigned j=0;j<P;j++) ck_sum[j] = sto_ptrs[z0][i];

		mul_alpha(ck_sum[1]);
		gf216bsv_mul_0x6e40(ck_sum[3]);
		ck_sum[0]^= sto_ptrs[z0-1][i];
		ck_sum[1]^= sto_ptrs[z0-1][i];
		ck_sum[4] = sto_ptrs[z0-1][i];
		ck_sum[3]^= sto_ptrs[z0-1][i];

		for( int z = z0-2; z >= 0 ; z-=2 ) {
			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[2]);
			gf216bsv_mul_0x6e40(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z][i];
			ck_sum[1]^=sto_ptrs[z][i];
			ck_sum[2]^=sto_ptrs[z][i];
			ck_sum[3]^=sto_ptrs[z][i];

			mul_alpha(ck_sum[1]);
			mul_alpha(ck_sum[4]);
			gf216bsv_mul_0x6e40(ck_sum[3]);
			ck_sum[0]^=sto_ptrs[z-1][i];
			ck_sum[1]^=sto_ptrs[z-1][i];
			ck_sum[3]^=sto_ptrs[z-1][i];
			ck_sum[4]^=sto_ptrs[z-1][i];
		}
		mul_sqrt_alpha(ck_sum[2]);
		ck_sum[2]^=ck_sum[4];
		for(unsigned j=0;j<P;j++) ck_sum[j] ^= ps[j][i];
		for(unsigned j=0;j<P;j++) {
			res = gf216bsv_mul( ck_sum[0] , rec_coef[j][0] );
			for( unsigned k=1;k<P;k++) res ^= gf216bsv_mul( ck_sum[k] , rec_coef[j][k] );
			fail_ptrs[j][i] = res;
		}
	}
	for(unsigned i=0;i<P;i++) {
		sto_ptrs[fail_idx[i]] = fail_ptrs[i];
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
