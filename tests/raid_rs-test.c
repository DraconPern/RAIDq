#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include <assert.h>

#include "stdint.h"

#include "raid_table.h"
#include "rs_table.h"
#include "raid_rs.h"



#define K 32
#define N8 (K+4)
#define N7 (K+3)
#define N6 (K+2)

#define IDXP (K)
#define IDXQ (K+1)
#define IDXR (K+2)
#define IDXS (K+3)


//const char raid6_empty_zero_page[PAGE_SIZE] __attribute__ ((aligned (256))) = {0};





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
	uint8_t fail_idx[4];
	fail_idx[0] = 3;
	fail_idx[1] = 6;
	fail_idx[2] = 9;
	fail_idx[3] = 13;

	uint8_t * mem[N8];
	uint8_t * mem2[N8];

	srand(23);

	for(i=0;i<N8;i++) if( 0 != posix_memalign( (void **) &mem[i] , 16, PAGE_SIZE) ) { printf("mem fail\n"); exit(-1); }
	for(i=0;i<N8;i++) if( 0 != posix_memalign( (void **) &mem2[i] , 16, PAGE_SIZE) ) { printf("mem fail\n"); exit(-1); }



	randomize_mem( mem , N8 );


///////////////////////////////////////////

	raid6_rs_ssse3x2_gen_syndrome(N6,PAGE_SIZE,(void**)mem);

	copy_mem( mem2 , mem , N6 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	/// checked code

	raid6_rs_ssse3x2_2fails_recov(N6,PAGE_SIZE,fail_idx[0],fail_idx[1],(void **)mem2 );
	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));



	raid6_rs_ssse3x2_2fails_recov(N6,PAGE_SIZE,fail_idx[0],fail_idx[1],(void**)mem2 );
	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));

	printf("raid6 passed.\n");

/////////////////////////////////////////////

	raid7_rs_ssse3x2_gen_syndrome(N7,PAGE_SIZE,(void**)mem);

//	fail_idx[0] = 2;
//	fail_idx[1] = 4;
//	fail_idx[2] = 6;

	copy_mem( mem2 , mem , N7 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	reset_mem( &mem2[fail_idx[2]] , 1 );
	/// checked code

	raid7_rs_ssse3x2_3fails_recov(N7,PAGE_SIZE,fail_idx[0],fail_idx[1],fail_idx[2],(void **)mem2 );

	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[2]][64],(uint64_t*)&mem2[fail_idx[2]][64]));


	printf("raid7 passed.\n");

/////////////////////////////////////////////

	raid8_rs_ssse3x2_gen_syndrome(N8,PAGE_SIZE,(void**)mem);

//
//	fail_idx[0] = 2;
//	fail_idx[1] = 4;
//	fail_idx[2] = 6;
//	fail_idx[2] = 8;

	copy_mem( mem2 , mem , N8 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	reset_mem( &mem2[fail_idx[2]] , 1 );
	reset_mem( &mem2[fail_idx[3]] , 1 );
	/// checked code

	raid8_rs_ssse3x2_4fails_recov(N8,PAGE_SIZE,fail_idx,(void **)mem2 );

	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[2]][64],(uint64_t*)&mem2[fail_idx[2]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[3]][64],(uint64_t*)&mem2[fail_idx[3]][64]));



	printf("raid8 passed.\n");


	return 0;
}

