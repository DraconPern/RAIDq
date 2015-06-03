#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include <assert.h>

#include "stdint.h"

#include "raid_table.h"
#include "raidq.h"

#include "raidq.hpp"


#define K 32
#define N8 (K+4)
#define N7 (K+3)
#define N6 (K+2)
#define N5 (K+1)

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
	fail_idx[0] = 2;
	fail_idx[1] = 6;
	fail_idx[2] = 7;
	fail_idx[3] = 13;

	uint8_t * mem[N8];
	uint8_t * mem2[N8];

	srand(23);

	for(i=0;i<N8;i++) if( 0 != posix_memalign( (void **) &mem[i] , 32, PAGE_SIZE) ) { printf("mem fail\n"); exit(-1); }
	for(i=0;i<N8;i++) if( 0 != posix_memalign( (void **) &mem2[i] , 32, PAGE_SIZE) ) { printf("mem fail\n"); exit(-1); }



	randomize_mem( mem , N8 );


///////////////////////////////////////////

	raid5_gen_syndrome<xmm_x2>(N5,PAGE_SIZE,(void**)mem);
	//raid5_xmmx2_gen_syndrome(N5,PAGE_SIZE,(void**)mem);

	copy_mem( mem2 , mem , N5 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	/// checked code

	raid5_recov<xmm_x2>(N5,PAGE_SIZE,fail_idx[0],(void **)mem2 );
	//raid5_xmmx2_recov(N5,PAGE_SIZE,fail_idx[0],(void **)mem2 );
	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));

	printf("raid5 passed.\n");


///////////////////////////////////////////

	raid6_gen_syndrome<xmm_x2>(N6,PAGE_SIZE,(void**)mem);
	//raid6_xmmx2_gen_syndrome(N6,PAGE_SIZE,(void**)mem);

	copy_mem( mem2 , mem , N6 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	/// checked code

//	raid6_rs_xmmx2_2fails_recov(N6,PAGE_SIZE,fail_idx[0],fail_idx[1],(void**)mem2 );
	raid6_recov_data<xmm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem2);
	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));

	printf("raid6 passed.\n");

/////////////////////////////////////////////

	raidq7_a2_gen_syndrome<xmm_x2>(N7,PAGE_SIZE,(void**)mem);

//	fail_idx[0] = 2;
//	fail_idx[1] = 4;
//	fail_idx[2] = 6;

	copy_mem( mem2 , mem , N7 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	reset_mem( &mem2[fail_idx[2]] , 1 );
	/// checked code

//	raid7_rs_xmmx2_3fails_recov(N7,PAGE_SIZE,fail_idx[0],fail_idx[1],fail_idx[2],(void **)mem2 );
	raidq7_a2_recov_data<xmm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem2);

	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[2]][64],(uint64_t*)&mem2[fail_idx[2]][64]));

	printf("raidq7 a2 passed.\n");

/////////////////////////////////////////////////////////////////////


	//raidq7_a2_xmmx2_gen_syndrome(N7,PAGE_SIZE,(void**)mem);
	raidq7_sqrta_gen_syndrome2<xmm_x2>(N7,PAGE_SIZE,(void**)mem);

//	fail_idx[0] = 2;
//	fail_idx[1] = 4;
//	fail_idx[2] = 6;

	copy_mem( mem2 , mem , N7 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	reset_mem( &mem2[fail_idx[2]] , 1 );
	/// checked code

//	raid7_rs_xmmx2_3fails_recov(N7,PAGE_SIZE,fail_idx[0],fail_idx[1],fail_idx[2],(void **)mem2 );
	raidq7_sqrta_recov_data2<xmm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem2);

	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[2]][64],(uint64_t*)&mem2[fail_idx[2]][64]));

	printf("raidq7 sqrt2 passed.\n");

/////////////////////////////////////////////

//
//	fail_idx[0] = 2;
//	fail_idx[1] = 4;
//	fail_idx[2] = 6;
//	fail_idx[2] = 8;

	typedef ymm_x2 tst_t;

	raidq8_X_gen_syndrome<tst_t>(N8,PAGE_SIZE,(void**)mem);

	copy_mem( mem2 , mem , N8 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	reset_mem( &mem2[fail_idx[2]] , 1 );
	reset_mem( &mem2[fail_idx[3]] , 1 );
	/// checked code

	raidq8_X_recov_data<tst_t>(K,PAGE_SIZE,fail_idx,(void **)mem2 );

	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[2]][64],(uint64_t*)&mem2[fail_idx[2]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[3]][64],(uint64_t*)&mem2[fail_idx[3]][64]));


////////////////////

	raidq8_a2Xp1_gen_syndrome<tst_t>(N8,PAGE_SIZE,(void**)mem);

	copy_mem( mem2 , mem , N8 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	reset_mem( &mem2[fail_idx[2]] , 1 );
	reset_mem( &mem2[fail_idx[3]] , 1 );
	/// checked code

	raidq8_a2Xp1_recov_data<tst_t>(K,PAGE_SIZE,fail_idx,(void **)mem2 );

	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[2]][64],(uint64_t*)&mem2[fail_idx[2]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[3]][64],(uint64_t*)&mem2[fail_idx[3]][64]));


////////////////////////

	raidq8_a141X_gen_syndrome<tst_t>(N8,PAGE_SIZE,(void**)mem);

	copy_mem( mem2 , mem , N8 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	reset_mem( &mem2[fail_idx[2]] , 1 );
	reset_mem( &mem2[fail_idx[3]] , 1 );
	/// checked code

	raidq8_a141X_recov_data<tst_t>(K,PAGE_SIZE,fail_idx,(void **)mem2 );

	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[2]][64],(uint64_t*)&mem2[fail_idx[2]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[3]][64],(uint64_t*)&mem2[fail_idx[3]][64]));


////////////////////////

	raidq8_a186Xpa6_gen_syndrome<tst_t>(N8,PAGE_SIZE,(void**)mem);

	copy_mem( mem2 , mem , N8 );
	reset_mem( &mem2[fail_idx[0]] , 1 );
	reset_mem( &mem2[fail_idx[1]] , 1 );
	reset_mem( &mem2[fail_idx[2]] , 1 );
	reset_mem( &mem2[fail_idx[3]] , 1 );
	/// checked code

	raidq8_a186Xpa6_recov_data<tst_t>(K,PAGE_SIZE,fail_idx,(void **)mem2 );

	assert(0==is_diff((uint64_t*)&mem[fail_idx[0]][64],(uint64_t*)&mem2[fail_idx[0]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[1]][64],(uint64_t*)&mem2[fail_idx[1]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[2]][64],(uint64_t*)&mem2[fail_idx[2]][64]));
	assert(0==is_diff((uint64_t*)&mem[fail_idx[3]][64],(uint64_t*)&mem2[fail_idx[3]][64]));

	printf("raid8 X passed.\n");


	return 0;
}

