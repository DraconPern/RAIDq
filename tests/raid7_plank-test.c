#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include <assert.h>

#include "stdint.h"

#include "raid_table.h"
#include "raid7_plank.h"



#define N 18
#define N7 (N+1)

#define IDXP (N-2)
#define IDXQ (N-1)
#define IDXR (N)
#define IDXS (N+1)


const char raid6_empty_zero_page[PAGE_SIZE] __attribute__ ((aligned (256))) = {0};


uint64_t is_diff( const uint64_t * a , const uint64_t * b )
{
	return a[0]^b[0];
}



void randomize_mem( uint8_t ** mem , unsigned n_pages )
{
	unsigned i,j;
	for(i=0;i<n_pages;i++) {
		for(j=0;j<PAGE_SIZE;j++) mem[i][j] = rand()&0xff;
	}
}


void reset_mem( uint8_t ** mem , unsigned n_pages )
{
	unsigned i,j;
	for(i=0;i<n_pages;i++) {
		for(j=0;j<PAGE_SIZE;j++) mem[i][j] = 0;
	}
}


void copy_mem( uint8_t ** mem2 , uint8_t ** mem , unsigned n_pages )
{
	unsigned i,j;
	for(i=0;i<n_pages;i++) {
		for(j=0;j<PAGE_SIZE;j++) mem2[i][j] = mem[i][j];
	}
}




int main()
{

	unsigned i;
	int fail_idx[4];
	fail_idx[0] = 1;
	fail_idx[1] = 3;
	fail_idx[2] = 5;
	fail_idx[3] = 7;

	uint8_t * mem[N7];
	uint8_t * mem2[N+2];

	srand(23);

	for(i=0;i<N7;i++) if( 0 != posix_memalign( (void **) &mem[i] , 16, PAGE_SIZE) ) { printf("mem fail\n"); exit(-1); }
	for(i=0;i<N+2;i++) if( 0 != posix_memalign( (void **) &mem2[i] , 16, PAGE_SIZE) ) { printf("mem fail\n"); exit(-1); }

	randomize_mem( mem , N-2);
	raid7_ssse3x2_gen_syndrome(N7,PAGE_SIZE,(void **)mem );

///////////////////////////////////

	copy_mem( mem2 , mem , N );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	/// checked code
	raid7_ssse3x2_1fail_recov(N7,PAGE_SIZE,fail_idx[1],(void **)mem2 );
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));


	copy_mem( mem2 , mem , N );
	reset_mem( &mem2[IDXP] , 1 );
	/// checked code
	raid7_ssse3x2_1fail_recov(N7,PAGE_SIZE,IDXP,(void **)mem2 );
	assert(0==is_diff((uint64_t*)&mem[IDXP][64],(uint64_t*)&mem2[IDXP][64]));


	copy_mem( mem2 , mem , N );
	reset_mem( &mem2[IDXQ] , 1 );
	/// checked code
	raid7_ssse3x2_1fail_recov(N7,PAGE_SIZE,IDXQ,(void **)mem2 );
	assert(0==is_diff((uint64_t*)&mem[IDXQ][64],(uint64_t*)&mem2[IDXQ][64]));

///////////////////////////////////////////


	copy_mem( mem2 , mem , N );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[IDXP] , 1 );
	/// checked code
//	raid6_ssse3x2_datap_recov(N,PAGE_SIZE,fail_idx[0],(void **)mem2 );
	raid7_ssse3x2_2fails_recov(N7,PAGE_SIZE,fail_idx[0],IDXP,(void **)mem2 );
	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[IDXP][64],(uint64_t*)&mem2[IDXP][64]));


	copy_mem( mem2 , mem , N );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[IDXQ] , 1 );
	/// checked code
//	raid6_ssse3x2_2data_recov(N,PAGE_SIZE,fail_idx[0],fail_idx[1],(void **)mem2 );
	raid7_ssse3x2_2fails_recov(N7,PAGE_SIZE,IDXQ,fail_idx[0],(void **)mem2 );
	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[IDXQ][64],(uint64_t*)&mem2[IDXQ][64]));


	copy_mem( mem2 , mem , N );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	/// checked code
//	raid6_ssse3x2_2data_recov(N,PAGE_SIZE,fail_idx[0],fail_idx[1],(void **)mem2 );
	raid7_ssse3x2_2fails_recov(N7,PAGE_SIZE,fail_idx[0],fail_idx[1],(void **)mem2 );
	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));


	printf("raid6 passed.\n");



	copy_mem( mem2 , mem , N7 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	reset_mem( &mem2[fail_idx[2]] , 1 );
	/// checked code
//	raid6_ssse3x2_2data_recov(N,PAGE_SIZE,fail_idx[0],fail_idx[1],(void **)mem2 );
	raid7_ssse3x2_3fails_recov(N7,PAGE_SIZE,fail_idx[0],fail_idx[1],fail_idx[2],(void **)mem2 );
	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[2]][64],(uint64_t*)&mem2[fail_idx[2]][64]));


	printf("raid7 passed.\n");


	return 0;
}

