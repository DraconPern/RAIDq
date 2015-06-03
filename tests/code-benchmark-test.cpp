#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include <assert.h>

#include "stdint.h"

#include "raid_table.h"
#include "rs_table.h"

#include "raid_rs.h"
#include "rs_table.h"
#include "raid_rs.hpp"

#include "raidq.h"

#include "raidq.hpp"

#include "benchmark.h"

/// GHz
#define CLOCK 3.4

#define TEST_RUN 10000

#define K 64
#define N8 (K+4)
#define N7 (K+3)
#define N6 (K+2)
#define N5 (K+1)

#define IDXP (K)
#define IDXQ (K+1)
#define IDXR (K+2)
#define IDXS (K+3)



#define BM_R5

#define BM_R6
#define BM_N6
#define BM_RS6

#define BM_RS7
#define BM_N7
#define BM_RQ7_A2
#define BM_RQ7_SQRTA2

#define BM_N8
#define BM_RQ8_X
#define BM_RQ8_A2XP1
#define BM_RQ8_A141X
#define BM_A186XPA6
#define BM_RS8



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


void dump(struct benchmark & bm , const char * str , int k , int m )
{
	char buf[1024];
	printf("%s , (%d+%d) code\n",str,k,m);
	bm_dump(buf, sizeof(buf), &bm);
	printf("%s : %s\n" , str , buf);

	double r_datasize = k*PAGE_SIZE;
	double w_datasize = m*PAGE_SIZE;
	printf("r data size: %f , throughput: %f GByte/sec\n", r_datasize , r_datasize*TEST_RUN*CLOCK/bm.acc );
	printf("w data size: %f , throughput: %f GByte/sec\n", w_datasize , w_datasize*TEST_RUN*CLOCK/bm.acc );
	printf("\n");
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
	//uint8_t * mem2[N8];


	srand(23);

	for(i=0;i<N8;i++) if( 0 != posix_memalign( (void **) &mem[i] , 32, PAGE_SIZE) ) { printf("mem fail\n"); exit(-1); }

	randomize_mem( mem , N8 );

	/* warming up */
	for(i=0;i<N8-1;i++) {
		for(unsigned j=0;j<PAGE_SIZE;j++) {
			mem[N8-1][j] ^= raid6_empty_zero_page[j];
			mem[N8-1][j] ^= mem[i][j];
		}
	}


///////////////////////////////////////////////

	printf("================================\n");
	printf("     benchmarker for various codes\n");
	printf("================================\n\n");



///////////////////////////////////////////////

#ifdef BM_R5
	struct benchmark bm_r5_enc_u64;
	struct benchmark bm_r5_enc_xmm1;
	struct benchmark bm_r5_enc_xmm2;
	struct benchmark bm_r5_enc_xmm4;
	struct benchmark bm_r5_enc_ymm1;
	struct benchmark bm_r5_enc_ymm2;
	struct benchmark bm_r5_enc_ymm4;

	struct benchmark bm_r5_dec_u64;
	struct benchmark bm_r5_dec_xmm1;
	struct benchmark bm_r5_dec_xmm2;
	struct benchmark bm_r5_dec_xmm4;
	struct benchmark bm_r5_dec_ymm1;
	struct benchmark bm_r5_dec_ymm2;
	struct benchmark bm_r5_dec_ymm4;

	bm_init(&bm_r5_enc_u64);
	bm_init(&bm_r5_enc_xmm1);
	bm_init(&bm_r5_enc_xmm2);
	bm_init(&bm_r5_enc_xmm4);
	bm_init(&bm_r5_enc_ymm1);
	bm_init(&bm_r5_enc_ymm2);
	bm_init(&bm_r5_enc_ymm4);
	bm_init(&bm_r5_dec_u64);
	bm_init(&bm_r5_dec_xmm1);
	bm_init(&bm_r5_dec_xmm2);
	bm_init(&bm_r5_dec_xmm4);
	bm_init(&bm_r5_dec_ymm1);
	bm_init(&bm_r5_dec_ymm2);
	bm_init(&bm_r5_dec_ymm4);

	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_r5_enc_u64 ,
		raid5_uint64_gen_syndrome(N5,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_r5_enc_xmm1 ,
		raid5_xmmx1_gen_syndrome(N5,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_r5_enc_xmm2 ,
		raid5_xmmx2_gen_syndrome(N5,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_r5_enc_xmm4 ,
		raid5_xmmx4_gen_syndrome(N5,PAGE_SIZE,(void**)mem)
);

BENCHMARK( bm_r5_enc_ymm1 ,
		raid5_gen_syndrome<ymm_x1>(N5,PAGE_SIZE,(void**)mem)
);

BENCHMARK( bm_r5_enc_ymm2 ,
		raid5_gen_syndrome<ymm_x2>(N5,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_r5_enc_ymm4 ,
		raid5_gen_syndrome<ymm_x4>(N5,PAGE_SIZE,(void**)mem)
);

BENCHMARK( bm_r5_dec_u64 ,
	raid5_uint64_recov(N5,PAGE_SIZE,fail_idx[0],(void **)mem );
);
BENCHMARK( bm_r5_dec_xmm1 ,
	raid5_xmmx1_recov(N5,PAGE_SIZE,fail_idx[0],(void **)mem );
);
BENCHMARK( bm_r5_dec_xmm2 ,
	raid5_xmmx2_recov(N5,PAGE_SIZE,fail_idx[0],(void **)mem )
);
BENCHMARK( bm_r5_dec_xmm4 ,
	raid5_xmmx4_recov(N5,PAGE_SIZE,fail_idx[0],(void **)mem );
);
BENCHMARK( bm_r5_dec_ymm1 ,
	raid5_recov<ymm_x1>(N5,PAGE_SIZE,fail_idx[0],(void **)mem );
);
BENCHMARK( bm_r5_dec_ymm2 ,
	raid5_recov<ymm_x2>(N5,PAGE_SIZE,fail_idx[0],(void **)mem )
);
BENCHMARK( bm_r5_dec_ymm4 ,
	raid5_recov<ymm_x4>(N5,PAGE_SIZE,fail_idx[0],(void **)mem );
);
	}

	printf("\n====================================\n");
	printf("RAID5:\n\n");

	dump( bm_r5_enc_u64 , "R5 u64 " , K , 1 );
	dump( bm_r5_enc_xmm1 , "R5 xmm x 1 " , K , 1 );
	dump( bm_r5_enc_xmm2 , "R5 xmm x 2 " , K , 1 );
	dump( bm_r5_enc_xmm4 , "R5 xmm x 4 " , K , 1 );
	dump( bm_r5_enc_ymm1 , "R5 ymm x 1 " , K , 1 );
	dump( bm_r5_enc_ymm2 , "R5 ymm x 2 " , K , 1 );
	dump( bm_r5_enc_ymm4 , "R5 ymm x 4 " , K , 1 );

	dump( bm_r5_dec_u64 , "R5 decode u64 " , K , 1 );
	dump( bm_r5_dec_xmm1 , "R5 decode xmm x 1 " , K , 1 );
	dump( bm_r5_dec_xmm2 , "R5 decode xmm x 2 " , K , 1 );
	dump( bm_r5_dec_xmm4 , "R5 decode xmm x 4 " , K , 1 );
	dump( bm_r5_dec_ymm1 , "R5 decode ymm x 1 " , K , 1 );
	dump( bm_r5_dec_ymm2 , "R5 decode ymm x 2 " , K , 1 );
	dump( bm_r5_dec_ymm4 , "R5 decode ymm x 4 " , K , 1 );


#endif



///////////////////////////////////////////

#ifdef BM_N6

	struct benchmark bm_n6_enc_xmm1;
	struct benchmark bm_n6_enc_xmm2;
	struct benchmark bm_n6_enc_xmm4;
	struct benchmark bm_n6_enc_ymm1;
	struct benchmark bm_n6_enc_ymm2;
	struct benchmark bm_n6_enc_ymm4;
	bm_init(&bm_n6_enc_xmm1);
	bm_init(&bm_n6_enc_xmm2);
	bm_init(&bm_n6_enc_xmm4);
	bm_init(&bm_n6_enc_ymm1);
	bm_init(&bm_n6_enc_ymm2);
	bm_init(&bm_n6_enc_ymm4);

	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_n6_enc_xmm1 ,
		naive6_gen_syndrome<__m128i>(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n6_enc_xmm2 ,
		naive6_gen_syndrome<xmm_x2>(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n6_enc_xmm4 ,
		naive6_gen_syndrome<xmm_x4>(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n6_enc_ymm1 ,
		naive6_gen_syndrome<ymm_x1>(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n6_enc_ymm2 ,
		naive6_gen_syndrome<ymm_x2>(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n6_enc_ymm4 ,
		naive6_gen_syndrome<ymm_x4>(N6,PAGE_SIZE,(void**)mem)
);
	}

	printf("\n====================================\n");
	printf("NAIVE 6:\n\n");

	dump( bm_n6_enc_xmm1 , "N6 xmm x 1 " , K , 2 );
	dump( bm_n6_enc_xmm2 , "N6 xmm x 2 " , K , 2 );
	dump( bm_n6_enc_xmm4 , "N6 xmm x 4 " , K , 2 );
	dump( bm_n6_enc_ymm1 , "N6 ymm x 1 " , K , 2 );
	dump( bm_n6_enc_ymm2 , "N6 ymm x 2 " , K , 2 );
	dump( bm_n6_enc_ymm4 , "N6 ymm x 4 " , K , 2 );

#endif

/////////////////////////////////////////////////

#ifdef BM_R6

	struct benchmark bm_r6_enc_xmm1;
	struct benchmark bm_r6_enc_xmm2;
	struct benchmark bm_r6_enc_xmm4;
	struct benchmark bm_r6_enc_ymm1;
	struct benchmark bm_r6_enc_ymm2;
	struct benchmark bm_r6_enc_ymm4;

	struct benchmark bm_r6_dec_xmm1;
	struct benchmark bm_r6_dec_xmm2;
	struct benchmark bm_r6_dec_xmm4;
	struct benchmark bm_r6_dec_ymm1;
	struct benchmark bm_r6_dec_ymm2;
	struct benchmark bm_r6_dec_ymm4;

	bm_init(&bm_r6_enc_xmm1);
	bm_init(&bm_r6_enc_xmm2);
	bm_init(&bm_r6_enc_xmm4);
	bm_init(&bm_r6_enc_ymm1);
	bm_init(&bm_r6_enc_ymm2);
	bm_init(&bm_r6_enc_ymm4);
	bm_init(&bm_r6_dec_xmm1);
	bm_init(&bm_r6_dec_xmm2);
	bm_init(&bm_r6_dec_xmm4);
	bm_init(&bm_r6_dec_ymm1);
	bm_init(&bm_r6_dec_ymm2);
	bm_init(&bm_r6_dec_ymm4);



	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_r6_enc_xmm1 ,
		raid6_xmmx1_gen_syndrome(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_r6_enc_xmm2 ,
		raid6_xmmx2_gen_syndrome(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_r6_enc_xmm4 ,
		raid6_xmmx4_gen_syndrome(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_r6_enc_ymm1 ,
		raid6_gen_syndrome<ymm_x1>(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_r6_enc_ymm2 ,
		raid6_gen_syndrome<ymm_x2>(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_r6_enc_ymm4 ,
		raid6_gen_syndrome<ymm_x4>(N6,PAGE_SIZE,(void**)mem)
);

BENCHMARK( bm_r6_dec_xmm1 ,
	raid6_recov_data<__m128i>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_r6_dec_xmm2 ,
	raid6_recov_data<xmm_x2>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_r6_dec_xmm4 ,
	raid6_recov_data<xmm_x4>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_r6_dec_ymm1 ,
	raid6_recov_data<ymm_x1>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_r6_dec_ymm2 ,
	raid6_recov_data<ymm_x2>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_r6_dec_ymm4 ,
	raid6_recov_data<ymm_x4>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
	}

	printf("\n====================================\n");
	printf("RAID6:\n\n");

	dump( bm_r6_enc_xmm1 , "R6 xmm x 1 " , K , 2 );
	dump( bm_r6_enc_xmm2 , "R6 xmm x 2 " , K , 2 );
	dump( bm_r6_enc_xmm4 , "R6 xmm x 4 " , K , 2 );

	dump( bm_r6_enc_ymm1 , "R6 ymm x 1 " , K , 2 );
	dump( bm_r6_enc_ymm2 , "R6 ymm x 2 " , K , 2 );
	dump( bm_r6_enc_ymm4 , "R6 ymm x 4 " , K , 2 );

	dump( bm_r6_dec_xmm1 , "R6 decode xmm x 1 " , K , 2 );
	dump( bm_r6_dec_xmm2 , "R6 decode xmm x 2 " , K , 2 );
	dump( bm_r6_dec_xmm4 , "R6 decode xmm x 4 " , K , 2 );
	dump( bm_r6_dec_ymm1 , "R6 decode ymm x 1 " , K , 2 );
	dump( bm_r6_dec_ymm2 , "R6 decode ymm x 2 " , K , 2 );
	dump( bm_r6_dec_ymm4 , "R6 decode ymm x 4 " , K , 2 );

#endif


///////////////////////////////////////////////


///////////////////////////////////////////////

#ifdef BM_RS6

	struct benchmark bm_rs6_enc_xmm1;
	struct benchmark bm_rs6_enc_xmm2;
	struct benchmark bm_rs6_enc_xmm4;
	struct benchmark bm_rs6_enc_ymm1;
	struct benchmark bm_rs6_enc_ymm2;
	struct benchmark bm_rs6_enc_ymm4;
	struct benchmark bm_rs6_dec_xmm1;
	struct benchmark bm_rs6_dec_xmm2;
	struct benchmark bm_rs6_dec_xmm4;
	struct benchmark bm_rs6_dec_ymm1;
	struct benchmark bm_rs6_dec_ymm2;
	struct benchmark bm_rs6_dec_ymm4;
	bm_init(&bm_rs6_enc_xmm1);
	bm_init(&bm_rs6_enc_xmm2);
	bm_init(&bm_rs6_enc_xmm4);
	bm_init(&bm_rs6_enc_ymm1);
	bm_init(&bm_rs6_enc_ymm2);
	bm_init(&bm_rs6_enc_ymm4);
	bm_init(&bm_rs6_dec_xmm1);
	bm_init(&bm_rs6_dec_xmm2);
	bm_init(&bm_rs6_dec_xmm4);
	bm_init(&bm_rs6_dec_ymm1);
	bm_init(&bm_rs6_dec_ymm2);
	bm_init(&bm_rs6_dec_ymm4);

	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_rs6_enc_xmm1 ,
		raid6_rs_ssse3x1_gen_syndrome(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rs6_enc_xmm2 ,
		raid6_rs_ssse3x2_gen_syndrome(N6,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rs6_enc_xmm4 ,
		raid6_rs_ssse3x4_gen_syndrome(N6,PAGE_SIZE,(void**)mem)
);

BENCHMARK( bm_rs6_enc_ymm1 ,
		(gen_syndrome<ymm_x1,2>(N6,PAGE_SIZE,(void**)mem,raid_rs))
);

BENCHMARK( bm_rs6_enc_ymm2 ,
		(gen_syndrome<ymm_x2,2>(N6,PAGE_SIZE,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs6_enc_ymm4 ,
		(gen_syndrome<ymm_x4,2>(N6,PAGE_SIZE,(void**)mem,raid_rs))
);

BENCHMARK( bm_rs6_dec_xmm1 ,
	raid6_rs_ssse3x1_2fails_recov(N6,PAGE_SIZE,fail_idx[0],fail_idx[1],(void **)mem )
);
BENCHMARK( bm_rs6_dec_xmm2 ,
	raid6_rs_ssse3x2_2fails_recov(N6,PAGE_SIZE,fail_idx[0],fail_idx[1],(void **)mem )
);
BENCHMARK( bm_rs6_dec_xmm4 ,
	raid6_rs_ssse3x4_2fails_recov(N6,PAGE_SIZE,fail_idx[0],fail_idx[1],(void **)mem )
);

BENCHMARK( bm_rs6_dec_ymm1 ,
		(raid_rs_recov_data<ymm_x1,2>(K,PAGE_SIZE,fail_idx,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs6_dec_ymm2 ,
		(raid_rs_recov_data<ymm_x2,2>(K,PAGE_SIZE,fail_idx,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs6_dec_ymm4 ,
		(raid_rs_recov_data<ymm_x4,2>(K,PAGE_SIZE,fail_idx,(void**)mem,raid_rs))
);

	}

	printf("\n=======================\n");
	printf("RS RAID6:\n\n");

	dump( bm_rs6_enc_xmm1 , "RS 6 xmm x 1 " , K , 2 );
	dump( bm_rs6_enc_xmm2 , "RS 6 xmm x 2 " , K , 2 );
	dump( bm_rs6_enc_xmm4 , "RS 6 xmm x 4 " , K , 2 );
	dump( bm_rs6_enc_ymm1 , "RS 6 ymm x 1 " , K , 2 );
	dump( bm_rs6_enc_ymm2 , "RS 6 ymm x 2 " , K , 2 );
	dump( bm_rs6_enc_ymm4 , "RS 6 ymm x 4 " , K , 2 );

	dump( bm_rs6_dec_xmm1 , "RS 6 decode xmm x 1 " , K , 2 );
	dump( bm_rs6_dec_xmm2 , "RS 6 decode xmm x 2 " , K , 2 );
	dump( bm_rs6_dec_xmm4 , "RS 6 decode xmm x 4 " , K , 2 );
	dump( bm_rs6_dec_ymm1 , "RS 6 decode ymm x 1 " , K , 2 );
	dump( bm_rs6_dec_ymm2 , "RS 6 decode ymm x 2 " , K , 2 );
	dump( bm_rs6_dec_ymm4 , "RS 6 decode ymm x 4 " , K , 2 );

#endif


/////////////////////////////////////////////////


#ifdef BM_N7

	struct benchmark bm_n7_enc_xmm1;
	struct benchmark bm_n7_enc_xmm2;
	struct benchmark bm_n7_enc_xmm4;
	struct benchmark bm_n7_enc_ymm1;
	struct benchmark bm_n7_enc_ymm2;
	struct benchmark bm_n7_enc_ymm4;

	bm_init(&bm_n7_enc_xmm1);
	bm_init(&bm_n7_enc_xmm2);
	bm_init(&bm_n7_enc_xmm4);
	bm_init(&bm_n7_enc_ymm1);
	bm_init(&bm_n7_enc_ymm2);
	bm_init(&bm_n7_enc_ymm4);



	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_n7_enc_xmm1 ,
		naive7_gen_syndrome<__m128i>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n7_enc_xmm2 ,
		naive7_gen_syndrome<xmm_x2>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n7_enc_xmm4 ,
		naive7_gen_syndrome<xmm_x4>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n7_enc_ymm1 ,
		naive7_gen_syndrome<ymm_x1>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n7_enc_ymm2 ,
		naive7_gen_syndrome<ymm_x2>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n7_enc_ymm4 ,
		naive7_gen_syndrome<ymm_x4>(N7,PAGE_SIZE,(void**)mem)
);
	}

	printf("\n====================================\n");
	printf("NAIVE 7:\n\n");

	dump( bm_n7_enc_xmm1 , "N7 xmm x 1 " , K , 3 );
	dump( bm_n7_enc_xmm2 , "N7 xmm x 2 " , K , 3 );
	dump( bm_n7_enc_xmm4 , "N7 xmm x 4 " , K , 3 );
	dump( bm_n7_enc_ymm1 , "N7 ymm x 1 " , K , 3 );
	dump( bm_n7_enc_ymm2 , "N7 ymm x 2 " , K , 3 );
	dump( bm_n7_enc_ymm4 , "N7 ymm x 4 " , K , 3 );

#endif


////////////////////////////////////////////////

#ifdef BM_RQ7_A2

	struct benchmark bm_a2_enc_xmm1;
	struct benchmark bm_a2_enc_xmm2;
	struct benchmark bm_a2_enc_xmm4;
	struct benchmark bm_a2_dec_xmm1;
	struct benchmark bm_a2_dec_xmm2;
	struct benchmark bm_a2_dec_xmm4;

	bm_init(&bm_a2_enc_xmm1);
	bm_init(&bm_a2_enc_xmm2);
	bm_init(&bm_a2_enc_xmm4);
	bm_init(&bm_a2_dec_xmm1);
	bm_init(&bm_a2_dec_xmm2);
	bm_init(&bm_a2_dec_xmm4);

	struct benchmark bm_a2_enc_ymm1;
	struct benchmark bm_a2_enc_ymm2;
	struct benchmark bm_a2_enc_ymm4;
	struct benchmark bm_a2_dec_ymm1;
	struct benchmark bm_a2_dec_ymm2;
	struct benchmark bm_a2_dec_ymm4;

	bm_init(&bm_a2_enc_ymm1);
	bm_init(&bm_a2_enc_ymm2);
	bm_init(&bm_a2_enc_ymm4);
	bm_init(&bm_a2_dec_ymm1);
	bm_init(&bm_a2_dec_ymm2);
	bm_init(&bm_a2_dec_ymm4);



	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_a2_enc_xmm1 ,
		raidq7_a2_gen_syndrome<__m128i>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a2_enc_xmm2 ,
		raidq7_a2_gen_syndrome<xmm_x2>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a2_enc_xmm4 ,
		raidq7_a2_gen_syndrome<xmm_x4>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a2_enc_ymm1 ,
		raidq7_a2_gen_syndrome<ymm_x1>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a2_enc_ymm2 ,
		raidq7_a2_gen_syndrome<ymm_x2>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a2_enc_ymm4 ,
		raidq7_a2_gen_syndrome<ymm_x4>(N7,PAGE_SIZE,(void**)mem)
);

BENCHMARK( bm_a2_dec_xmm1 ,
	raidq7_a2_recov_data<__m128i>(K,PAGE_SIZE,fail_idx,(void **)mem );
);
BENCHMARK( bm_a2_dec_xmm2 ,
	raidq7_a2_recov_data<xmm_x2>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_a2_dec_xmm4 ,
	raidq7_a2_recov_data<xmm_x4>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_a2_dec_ymm1 ,
	raidq7_a2_recov_data<ymm_x1>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_a2_dec_ymm2 ,
	raidq7_a2_recov_data<ymm_x2>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_a2_dec_ymm4 ,
	raidq7_a2_recov_data<ymm_x4>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
	}

	printf("\n====================================\n");
	printf("RAID7:\n\n");

	dump( bm_a2_enc_xmm1 , "RQ7 a2 xmm x 1 " , K , 3 );
	dump( bm_a2_enc_xmm2 , "RQ7 a2 xmm x 2 " , K , 3 );
	dump( bm_a2_enc_xmm4 , "RQ7 a2 xmm x 4 " , K , 3 );
	dump( bm_a2_enc_ymm1 , "RQ7 a2 ymm x 1 " , K , 3 );
	dump( bm_a2_enc_ymm2 , "RQ7 a2 ymm x 2 " , K , 3 );
	dump( bm_a2_enc_ymm4 , "RQ7 a2 ymm x 4 " , K , 3 );

	dump( bm_a2_dec_xmm1 , "RQ7 a2 decode xmm x 1 " , K , 3 );
	dump( bm_a2_dec_xmm2 , "RQ7 a2 decode xmm x 2 " , K , 3 );
	dump( bm_a2_dec_xmm4 , "RQ7 a2 decode xmm x 4 " , K , 3 );
	dump( bm_a2_dec_ymm1 , "RQ7 a2 decode ymm x 1 " , K , 3 );
	dump( bm_a2_dec_ymm2 , "RQ7 a2 decode ymm x 2 " , K , 3 );
	dump( bm_a2_dec_ymm4 , "RQ7 a2 decode ymm x 4 " , K , 3 );


#endif

/////////////////////////////////////////////

#ifdef BM_RQ7_SQRTA


	struct benchmark bm_rq7sqrta_enc_xmm1;
	struct benchmark bm_rq7sqrta_enc_xmm2;
	struct benchmark bm_rq7sqrta_enc_xmm4;
	struct benchmark bm_rq7sqrta_dec_xmm1;
	struct benchmark bm_rq7sqrta_dec_xmm2;
	struct benchmark bm_rq7sqrta_dec_xmm4;

	bm_init(&bm_rq7sqrta_enc_xmm1);
	bm_init(&bm_rq7sqrta_enc_xmm2);
	bm_init(&bm_rq7sqrta_enc_xmm4);
	bm_init(&bm_rq7sqrta_dec_xmm1);
	bm_init(&bm_rq7sqrta_dec_xmm2);
	bm_init(&bm_rq7sqrta_dec_xmm4);



	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_rq7sqrta_enc_xmm1 ,
		raidq7_sqrta_gen_syndrome<__m128i>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rq7sqrta_enc_xmm2 ,
		raidq7_sqrta_gen_syndrome<xmm_x2>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rq7sqrta_enc_xmm4 ,
		raidq7_sqrta_gen_syndrome<xmm_x4>(N7,PAGE_SIZE,(void**)mem)
);
/*
BENCHMARK( bm_rq7sqrta_dec_xmm1 ,
	raidq7_a2_xmmx1_3fails_recov(N7,PAGE_SIZE,fail_idx,(void **)mem );
);
BENCHMARK( bm_rq7sqrta_dec_xmm2 ,
	raidq7_a2_xmmx2_3fails_recov(N7,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_rq7sqrta_dec_xmm4 ,
	raidq7_a2_xmmx4_3fails_recov(N7,PAGE_SIZE,fail_idx,(void **)mem );
);
*/
	}

	printf("\n====================================\n");
	printf("RAID7:\n\n");

	dump( bm_rq7sqrta_enc_xmm1 , "RQ7 sqrta xmm x 1 " , K , 3 );
	dump( bm_rq7sqrta_enc_xmm2 , "RQ7 sqrta xmm x 2 " , K , 3 );
	dump( bm_rq7sqrta_enc_xmm4 , "RQ7 sqrta xmm x 4 " , K , 3 );

	dump( bm_rq7sqrta_dec_xmm1 , "RQ7 sqrta decode xmm x 1 " , K , 3 );
	dump( bm_rq7sqrta_dec_xmm2 , "RQ7 sqrta decode xmm x 2 " , K , 3 );
	dump( bm_rq7sqrta_dec_xmm4 , "RQ7 sqrta decode xmm x 4 " , K , 3 );


#endif


/////////////////////////////////////////////


#ifdef BM_RQ7_SQRTA2

	struct benchmark bm_rq7sqrta2_enc_xmm1;
	struct benchmark bm_rq7sqrta2_enc_xmm2;
	struct benchmark bm_rq7sqrta2_enc_xmm4;
	struct benchmark bm_rq7sqrta2_dec_xmm1;
	struct benchmark bm_rq7sqrta2_dec_xmm2;
	struct benchmark bm_rq7sqrta2_dec_xmm4;

	struct benchmark bm_rq7sqrta2_enc_ymm1;
	struct benchmark bm_rq7sqrta2_enc_ymm2;
	struct benchmark bm_rq7sqrta2_enc_ymm4;
	struct benchmark bm_rq7sqrta2_dec_ymm1;
	struct benchmark bm_rq7sqrta2_dec_ymm2;
	struct benchmark bm_rq7sqrta2_dec_ymm4;

	bm_init(&bm_rq7sqrta2_enc_xmm1);
	bm_init(&bm_rq7sqrta2_enc_xmm2);
	bm_init(&bm_rq7sqrta2_enc_xmm4);
	bm_init(&bm_rq7sqrta2_dec_xmm1);
	bm_init(&bm_rq7sqrta2_dec_xmm2);
	bm_init(&bm_rq7sqrta2_dec_xmm4);

	bm_init(&bm_rq7sqrta2_enc_ymm1);
	bm_init(&bm_rq7sqrta2_enc_ymm2);
	bm_init(&bm_rq7sqrta2_enc_ymm4);
	bm_init(&bm_rq7sqrta2_dec_ymm1);
	bm_init(&bm_rq7sqrta2_dec_ymm2);
	bm_init(&bm_rq7sqrta2_dec_ymm4);



	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_rq7sqrta2_enc_xmm1 ,
		raidq7_sqrta_gen_syndrome2<__m128i>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rq7sqrta2_enc_xmm2 ,
		raidq7_sqrta_gen_syndrome2<xmm_x2>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rq7sqrta2_enc_xmm4 ,
		raidq7_sqrta_gen_syndrome2<xmm_x4>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rq7sqrta2_enc_ymm1 ,
		raidq7_sqrta_gen_syndrome2<ymm_x1>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rq7sqrta2_enc_ymm2 ,
		raidq7_sqrta_gen_syndrome2<ymm_x2>(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rq7sqrta2_enc_ymm4 ,
		raidq7_sqrta_gen_syndrome2<ymm_x4>(N7,PAGE_SIZE,(void**)mem)
);

BENCHMARK( bm_rq7sqrta2_dec_xmm1 ,
	raidq7_sqrta_recov_data2<__m128i>(K,PAGE_SIZE,fail_idx,(void **)mem );
);
BENCHMARK( bm_rq7sqrta2_dec_xmm2 ,
	raidq7_sqrta_recov_data2<xmm_x2>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_rq7sqrta2_dec_xmm4 ,
	raidq7_sqrta_recov_data2<xmm_x4>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_rq7sqrta2_dec_ymm1 ,
	raidq7_sqrta_recov_data2<ymm_x1>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_rq7sqrta2_dec_ymm2 ,
	raidq7_sqrta_recov_data2<ymm_x2>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_rq7sqrta2_dec_ymm4 ,
	raidq7_sqrta_recov_data2<ymm_x4>(K,PAGE_SIZE,fail_idx,(void **)mem )
);
	}

	printf("\n====================================\n");
	printf("RAID7:\n\n");

	dump( bm_rq7sqrta2_enc_xmm1 , "RQ7 sqrta2 xmm x 1 " , K , 3 );
	dump( bm_rq7sqrta2_enc_xmm2 , "RQ7 sqrta2 xmm x 2 " , K , 3 );
	dump( bm_rq7sqrta2_enc_xmm4 , "RQ7 sqrta2 xmm x 4 " , K , 3 );

	dump( bm_rq7sqrta2_enc_ymm1 , "RQ7 sqrta2 ymm x 1 " , K , 3 );
	dump( bm_rq7sqrta2_enc_ymm2 , "RQ7 sqrta2 ymm x 2 " , K , 3 );
	dump( bm_rq7sqrta2_enc_ymm4 , "RQ7 sqrta2 ymm x 4 " , K , 3 );

	dump( bm_rq7sqrta2_dec_xmm1 , "RQ7 sqrta2 decode xmm x 1 " , K , 3 );
	dump( bm_rq7sqrta2_dec_xmm2 , "RQ7 sqrta2 decode xmm x 2 " , K , 3 );
	dump( bm_rq7sqrta2_dec_xmm4 , "RQ7 sqrta2 decode xmm x 4 " , K , 3 );

	dump( bm_rq7sqrta2_dec_ymm1 , "RQ7 sqrta2 decode ymm x 1 " , K , 3 );
	dump( bm_rq7sqrta2_dec_ymm2 , "RQ7 sqrta2 decode ymm x 2 " , K , 3 );
	dump( bm_rq7sqrta2_dec_ymm4 , "RQ7 sqrta2 decode ymm x 4 " , K , 3 );


#endif



/////////////////////////////////////////////


#ifdef BM_RS7


	struct benchmark bm_rs7_enc_xmm1;
	struct benchmark bm_rs7_enc_xmm2;
	struct benchmark bm_rs7_enc_xmm4;
	struct benchmark bm_rs7_dec_xmm1;
	struct benchmark bm_rs7_dec_xmm2;
	struct benchmark bm_rs7_dec_xmm4;

	struct benchmark bm_rs7_enc_ymm1;
	struct benchmark bm_rs7_enc_ymm2;
	struct benchmark bm_rs7_enc_ymm4;
	struct benchmark bm_rs7_dec_ymm1;
	struct benchmark bm_rs7_dec_ymm2;
	struct benchmark bm_rs7_dec_ymm4;

	bm_init(&bm_rs7_enc_xmm1);
	bm_init(&bm_rs7_enc_xmm2);
	bm_init(&bm_rs7_enc_xmm4);
	bm_init(&bm_rs7_dec_xmm1);
	bm_init(&bm_rs7_dec_xmm2);
	bm_init(&bm_rs7_dec_xmm4);

	bm_init(&bm_rs7_enc_ymm1);
	bm_init(&bm_rs7_enc_ymm2);
	bm_init(&bm_rs7_enc_ymm4);
	bm_init(&bm_rs7_dec_ymm1);
	bm_init(&bm_rs7_dec_ymm2);
	bm_init(&bm_rs7_dec_ymm4);


	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_rs7_enc_xmm1 ,
		raid7_rs_ssse3x1_gen_syndrome(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rs7_enc_xmm2 ,
		raid7_rs_ssse3x2_gen_syndrome(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rs7_enc_xmm4 ,
		raid7_rs_ssse3x4_gen_syndrome(N7,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rs7_enc_ymm1 ,
		(gen_syndrome<ymm_x1,3>(N7,PAGE_SIZE,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs7_enc_ymm2 ,
		(gen_syndrome<ymm_x2,3>(N7,PAGE_SIZE,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs7_enc_ymm4 ,
		(gen_syndrome<ymm_x4,3>(N7,PAGE_SIZE,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs7_dec_xmm1 ,
	raid7_rs_ssse3x1_3fails_recov(N7,PAGE_SIZE,fail_idx[0],fail_idx[1],fail_idx[2],(void **)mem );
);
BENCHMARK( bm_rs7_dec_xmm2 ,
	raid7_rs_ssse3x2_3fails_recov(N7,PAGE_SIZE,fail_idx[0],fail_idx[1],fail_idx[2],(void **)mem )
);
BENCHMARK( bm_rs7_dec_xmm4 ,
	raid7_rs_ssse3x4_3fails_recov(N7,PAGE_SIZE,fail_idx[0],fail_idx[1],fail_idx[2],(void **)mem );
);
BENCHMARK( bm_rs7_dec_ymm1 ,
		(raid_rs_recov_data<ymm_x1,3>(K,PAGE_SIZE,fail_idx,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs7_dec_ymm2 ,
		(raid_rs_recov_data<ymm_x2,3>(K,PAGE_SIZE,fail_idx,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs7_dec_ymm4 ,
		(raid_rs_recov_data<ymm_x4,3>(K,PAGE_SIZE,fail_idx,(void**)mem,raid_rs))
);

	}

	printf("\n====================================\n");
	printf("RS RAID7:\n\n");

	dump( bm_rs7_enc_xmm1 , "RS 7 xmm x 1 " , K , 3 );
	dump( bm_rs7_enc_xmm2 , "RS 7 xmm x 2 " , K , 3 );
	dump( bm_rs7_enc_xmm4 , "RS 7 xmm x 4 " , K , 3 );

	dump( bm_rs7_enc_ymm1 , "RS 7 ymm x 1 " , K , 3 );
	dump( bm_rs7_enc_ymm2 , "RS 7 ymm x 2 " , K , 3 );
	dump( bm_rs7_enc_ymm4 , "RS 7 ymm x 4 " , K , 3 );

	dump( bm_rs7_dec_xmm1 , "RS 7 decode xmm x 1 " , K , 3 );
	dump( bm_rs7_dec_xmm2 , "RS 7 decode xmm x 2 " , K , 3 );
	dump( bm_rs7_dec_xmm4 , "RS 7 decode xmm x 4 " , K , 3 );

	dump( bm_rs7_dec_ymm1 , "RS 7 decode ymm x 1 " , K , 3 );
	dump( bm_rs7_dec_ymm2 , "RS 7 decode ymm x 2 " , K , 3 );
	dump( bm_rs7_dec_ymm4 , "RS 7 decode ymm x 4 " , K , 3 );


#endif



/////////////////////////////////////////////////


#ifdef BM_N8

	struct benchmark bm_n8_enc_xmm1;
	struct benchmark bm_n8_enc_xmm2;
	struct benchmark bm_n8_enc_xmm4;

	bm_init(&bm_n8_enc_xmm1);
	bm_init(&bm_n8_enc_xmm2);
	bm_init(&bm_n8_enc_xmm4);


	struct benchmark bm_n8_enc_ymm1;
	struct benchmark bm_n8_enc_ymm2;
	struct benchmark bm_n8_enc_ymm4;

	bm_init(&bm_n8_enc_ymm1);
	bm_init(&bm_n8_enc_ymm2);
	bm_init(&bm_n8_enc_ymm4);



	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_n8_enc_xmm1 ,
		naive8_gen_syndrome<__m128i>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n8_enc_xmm2 ,
		naive8_gen_syndrome<xmm_x2>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n8_enc_xmm4 ,
		naive8_gen_syndrome<xmm_x4>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n8_enc_ymm1 ,
		naive8_gen_syndrome<ymm_x1>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n8_enc_ymm2 ,
		naive8_gen_syndrome<ymm_x2>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_n8_enc_ymm4 ,
		naive8_gen_syndrome<ymm_x4>(N8,PAGE_SIZE,(void**)mem)
);
	}

	printf("\n====================================\n");
	printf("NAIVE 8:\n\n");

	dump( bm_n8_enc_xmm1 , "N8 xmm x 1 " , K , 4 );
	dump( bm_n8_enc_xmm2 , "N8 xmm x 2 " , K , 4 );
	dump( bm_n8_enc_xmm4 , "N8 xmm x 4 " , K , 4 );
	dump( bm_n8_enc_ymm1 , "N8 ymm x 1 " , K , 4 );
	dump( bm_n8_enc_ymm2 , "N8 ymm x 2 " , K , 4 );
	dump( bm_n8_enc_ymm4 , "N8 ymm x 4 " , K , 4 );

#endif



/////////////////////////////////////////////


#ifdef BM_RQ8_X

	struct benchmark bm_X_enc_xmm2;
	struct benchmark bm_X_enc_xmm4;
	struct benchmark bm_X_dec_xmm2;
	struct benchmark bm_X_dec_xmm4;

	bm_init(&bm_X_enc_xmm2);
	bm_init(&bm_X_enc_xmm4);
	bm_init(&bm_X_dec_xmm2);
	bm_init(&bm_X_dec_xmm4);

	struct benchmark bm_X_enc_ymm2;
	struct benchmark bm_X_enc_ymm4;
	struct benchmark bm_X_dec_ymm2;
	struct benchmark bm_X_dec_ymm4;

	bm_init(&bm_X_enc_ymm2);
	bm_init(&bm_X_enc_ymm4);
	bm_init(&bm_X_dec_ymm2);
	bm_init(&bm_X_dec_ymm4);



	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_X_enc_xmm2 ,
		raidq8_X_gen_syndrome<xmm_x2>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_X_dec_xmm2 ,
		raidq8_X_recov_data<xmm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_X_enc_xmm4 ,
		raidq8_X_gen_syndrome<xmm_x4>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_X_dec_xmm4 ,
		raidq8_X_recov_data<xmm_x4>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_X_enc_ymm2 ,
		raidq8_X_gen_syndrome<ymm_x2>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_X_dec_ymm2 ,
		raidq8_X_recov_data<ymm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_X_enc_ymm4 ,
		raidq8_X_gen_syndrome<ymm_x4>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_X_dec_ymm4 ,
		raidq8_X_recov_data<ymm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
	}

	printf("\n====================================\n");
	printf("RAID Q 8 X:\n\n");

	dump( bm_X_enc_xmm2 , "RQ 8 X xmm x 2 " , K , 4 );
	dump( bm_X_enc_xmm4 , "RQ 8 X xmm x 4 " , K , 4 );
	dump( bm_X_enc_ymm2 , "RQ 8 X ymm x 2 " , K , 4 );
	dump( bm_X_enc_ymm4 , "RQ 8 X ymm x 4 " , K , 4 );

	dump( bm_X_dec_xmm2 , "RQ 8 X decode xmm x 2 " , K , 4 );
	dump( bm_X_dec_xmm4 , "RQ 8 X decode xmm x 4 " , K , 4 );
	dump( bm_X_dec_ymm2 , "RQ 8 X decode ymm x 2 " , K , 4 );
	dump( bm_X_dec_ymm4 , "RQ 8 X decode ymm x 4 " , K , 4 );


#endif




/////////////////////////////////////////////

#ifdef BM_RQ8_A2XP1


	struct benchmark bm_a2Xp1_enc_xmm2;
	struct benchmark bm_a2Xp1_enc_xmm4;
	struct benchmark bm_a2Xp1_dec_xmm2;
	struct benchmark bm_a2Xp1_dec_xmm4;

	bm_init(&bm_a2Xp1_enc_xmm2);
	bm_init(&bm_a2Xp1_enc_xmm4);
	bm_init(&bm_a2Xp1_dec_xmm2);
	bm_init(&bm_a2Xp1_dec_xmm4);

	struct benchmark bm_a2Xp1_enc_ymm2;
	struct benchmark bm_a2Xp1_enc_ymm4;
	struct benchmark bm_a2Xp1_dec_ymm2;
	struct benchmark bm_a2Xp1_dec_ymm4;

	bm_init(&bm_a2Xp1_enc_ymm2);
	bm_init(&bm_a2Xp1_enc_ymm4);
	bm_init(&bm_a2Xp1_dec_ymm2);
	bm_init(&bm_a2Xp1_dec_ymm4);



	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_a2Xp1_enc_xmm2 ,
		raidq8_a2Xp1_gen_syndrome<xmm_x2>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a2Xp1_dec_xmm2 ,
		raidq8_a2Xp1_recov_data<xmm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_a2Xp1_enc_xmm4 ,
		raidq8_a2Xp1_gen_syndrome<xmm_x4>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a2Xp1_dec_xmm4 ,
		raidq8_a2Xp1_recov_data<xmm_x4>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_a2Xp1_enc_ymm2 ,
		raidq8_a2Xp1_gen_syndrome<ymm_x2>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a2Xp1_dec_ymm2 ,
		raidq8_a2Xp1_recov_data<ymm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_a2Xp1_enc_ymm4 ,
		raidq8_a2Xp1_gen_syndrome<ymm_x4>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a2Xp1_dec_ymm4 ,
		raidq8_a2Xp1_recov_data<ymm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
	}

	printf("\n====================================\n");
	printf("RAID Q 8 a2X+1:\n\n");

	dump( bm_a2Xp1_enc_xmm2 , "RQ 8 a2X+1 xmm x 2 " , K , 4 );
	dump( bm_a2Xp1_enc_xmm4 , "RQ 8 a2X+1 xmm x 4 " , K , 4 );

	dump( bm_a2Xp1_enc_ymm2 , "RQ 8 a2X+1 ymm x 2 " , K , 4 );
	dump( bm_a2Xp1_enc_ymm4 , "RQ 8 a2X+1 ymm x 4 " , K , 4 );

	dump( bm_a2Xp1_dec_xmm2 , "RQ 8 a2X+1 decode xmm x 2 " , K , 4 );
	dump( bm_a2Xp1_dec_xmm4 , "RQ 8 a2X+1 decode xmm x 4 " , K , 4 );
	dump( bm_a2Xp1_dec_ymm2 , "RQ 8 a2X+1 decode ymm x 2 " , K , 4 );
	dump( bm_a2Xp1_dec_ymm4 , "RQ 8 a2X+1 decode ymm x 4 " , K , 4 );



#endif



/////////////////////////////////////////////


#ifdef BM_RQ8_A141X


	struct benchmark bm_a141X_enc_xmm2;
	struct benchmark bm_a141X_enc_xmm4;
	struct benchmark bm_a141X_dec_xmm2;
	struct benchmark bm_a141X_dec_xmm4;

	bm_init(&bm_a141X_enc_xmm2);
	bm_init(&bm_a141X_enc_xmm4);
	bm_init(&bm_a141X_dec_xmm2);
	bm_init(&bm_a141X_dec_xmm4);


	struct benchmark bm_a141X_enc_ymm2;
	struct benchmark bm_a141X_enc_ymm4;
	struct benchmark bm_a141X_dec_ymm2;
	struct benchmark bm_a141X_dec_ymm4;

	bm_init(&bm_a141X_enc_ymm2);
	bm_init(&bm_a141X_enc_ymm4);
	bm_init(&bm_a141X_dec_ymm2);
	bm_init(&bm_a141X_dec_ymm4);



	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_a141X_enc_xmm2 ,
		raidq8_a141X_gen_syndrome<xmm_x2>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a141X_dec_xmm2 ,
		raidq8_a141X_recov_data<xmm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_a141X_enc_xmm4 ,
		raidq8_a141X_gen_syndrome<xmm_x4>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a141X_dec_xmm4 ,
		raidq8_a141X_recov_data<xmm_x4>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_a141X_enc_ymm2 ,
		raidq8_a141X_gen_syndrome<ymm_x2>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a141X_dec_ymm2 ,
		raidq8_a141X_recov_data<ymm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_a141X_enc_ymm4 ,
		raidq8_a141X_gen_syndrome<ymm_x4>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a141X_dec_ymm4 ,
		raidq8_a141X_recov_data<ymm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
	}

	printf("\n====================================\n");
	printf("RAID Q 8 a141 X:\n\n");

	dump( bm_a141X_enc_xmm2 , "RQ 8 a141X xmm x 2 " , K , 4 );
	dump( bm_a141X_enc_xmm4 , "RQ 8 a141X xmm x 4 " , K , 4 );

	dump( bm_a141X_enc_ymm2 , "RQ 8 a141X ymm x 2 " , K , 4 );
	dump( bm_a141X_enc_ymm4 , "RQ 8 a141X ymm x 4 " , K , 4 );

	dump( bm_a141X_dec_xmm2 , "RQ 8 a141X decode xmm x 2 " , K , 4 );
	dump( bm_a141X_dec_xmm4 , "RQ 8 a141X decode xmm x 4 " , K , 4 );

	dump( bm_a141X_dec_ymm2 , "RQ 8 a141X decode ymm x 2 " , K , 4 );
	dump( bm_a141X_dec_ymm4 , "RQ 8 a141X decode ymm x 4 " , K , 4 );



#endif



/////////////////////////////////////////////


#ifdef BM_A186XPA6



	struct benchmark bm_a186Xpa6_enc_xmm2;
	struct benchmark bm_a186Xpa6_enc_xmm4;
	struct benchmark bm_a186Xpa6_dec_xmm2;
	struct benchmark bm_a186Xpa6_dec_xmm4;

	bm_init(&bm_a186Xpa6_enc_xmm2);
	bm_init(&bm_a186Xpa6_enc_xmm4);
	bm_init(&bm_a186Xpa6_dec_xmm2);
	bm_init(&bm_a186Xpa6_dec_xmm4);

	struct benchmark bm_a186Xpa6_enc_ymm2;
	struct benchmark bm_a186Xpa6_enc_ymm4;
	struct benchmark bm_a186Xpa6_dec_ymm2;
	struct benchmark bm_a186Xpa6_dec_ymm4;

	bm_init(&bm_a186Xpa6_enc_ymm2);
	bm_init(&bm_a186Xpa6_enc_ymm4);
	bm_init(&bm_a186Xpa6_dec_ymm2);
	bm_init(&bm_a186Xpa6_dec_ymm4);



	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_a186Xpa6_enc_xmm2 ,
		raidq8_a186Xpa6_gen_syndrome<xmm_x2>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a186Xpa6_dec_xmm2 ,
		raidq8_a186Xpa6_recov_data<xmm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_a186Xpa6_enc_xmm4 ,
		raidq8_a186Xpa6_gen_syndrome<xmm_x4>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a186Xpa6_dec_xmm4 ,
		raidq8_a186Xpa6_recov_data<xmm_x4>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_a186Xpa6_enc_ymm2 ,
		raidq8_a186Xpa6_gen_syndrome<ymm_x2>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a186Xpa6_dec_ymm2 ,
		raidq8_a186Xpa6_recov_data<ymm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
BENCHMARK( bm_a186Xpa6_enc_ymm4 ,
		raidq8_a186Xpa6_gen_syndrome<ymm_x4>(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_a186Xpa6_dec_ymm4 ,
		raidq8_a186Xpa6_recov_data<ymm_x2>(K,PAGE_SIZE,fail_idx,(void**)mem)
);
	}

	printf("\n====================================\n");
	printf("RAID Q 8 a186 X + a6:\n\n");

	dump( bm_a186Xpa6_enc_xmm2 , "RQ 8 a186X+a6 xmm x 2 " , K , 4 );
	dump( bm_a186Xpa6_enc_xmm4 , "RQ 8 a186X+a6 xmm x 4 " , K , 4 );

	dump( bm_a186Xpa6_enc_ymm2 , "RQ 8 a186X+a6 ymm x 2 " , K , 4 );
	dump( bm_a186Xpa6_enc_ymm4 , "RQ 8 a186X+a6 ymm x 4 " , K , 4 );

	dump( bm_a186Xpa6_dec_xmm2 , "RQ 8 a186X+a6 decode xmm x 2 " , K , 4 );
	dump( bm_a186Xpa6_dec_xmm4 , "RQ 8 a186X+a6 decode xmm x 4 " , K , 4 );
	dump( bm_a186Xpa6_dec_ymm2 , "RQ 8 a186X+a6 decode ymm x 2 " , K , 4 );
	dump( bm_a186Xpa6_dec_ymm4 , "RQ 8 a186X+a6 decode ymm x 4 " , K , 4 );



#endif



/////////////////////////////////////////////


#ifdef BM_RS8


	struct benchmark bm_rs8_enc_xmm1;
	struct benchmark bm_rs8_enc_xmm2;
	struct benchmark bm_rs8_enc_xmm4;
	struct benchmark bm_rs8_dec_xmm1;
	struct benchmark bm_rs8_dec_xmm2;
	struct benchmark bm_rs8_dec_xmm4;

	bm_init(&bm_rs8_enc_xmm1);
	bm_init(&bm_rs8_enc_xmm2);
	bm_init(&bm_rs8_enc_xmm4);
	bm_init(&bm_rs8_dec_xmm1);
	bm_init(&bm_rs8_dec_xmm2);
	bm_init(&bm_rs8_dec_xmm4);


	struct benchmark bm_rs8_enc_ymm1;
	struct benchmark bm_rs8_enc_ymm2;
	struct benchmark bm_rs8_enc_ymm4;
	struct benchmark bm_rs8_dec_ymm1;
	struct benchmark bm_rs8_dec_ymm2;
	struct benchmark bm_rs8_dec_ymm4;

	bm_init(&bm_rs8_enc_ymm1);
	bm_init(&bm_rs8_enc_ymm2);
	bm_init(&bm_rs8_enc_ymm4);
	bm_init(&bm_rs8_dec_ymm1);
	bm_init(&bm_rs8_dec_ymm2);
	bm_init(&bm_rs8_dec_ymm4);



	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_rs8_enc_xmm1 ,
		raid8_rs_ssse3x1_gen_syndrome(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rs8_enc_xmm2 ,
		raid8_rs_ssse3x2_gen_syndrome(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rs8_enc_xmm4 ,
		raid8_rs_ssse3x4_gen_syndrome(N8,PAGE_SIZE,(void**)mem)
);
BENCHMARK( bm_rs8_enc_ymm1 ,
		(gen_syndrome<ymm_x1,4>(N8,PAGE_SIZE,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs8_enc_ymm2 ,
		(gen_syndrome<ymm_x2,4>(N8,PAGE_SIZE,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs8_enc_ymm4 ,
		(gen_syndrome<ymm_x4,4>(N8,PAGE_SIZE,(void**)mem,raid_rs))
);

BENCHMARK( bm_rs8_dec_xmm1 ,
	raid8_rs_ssse3x1_4fails_recov(N8,PAGE_SIZE,fail_idx,(void **)mem );
);
BENCHMARK( bm_rs8_dec_xmm2 ,
	raid8_rs_ssse3x2_4fails_recov(N8,PAGE_SIZE,fail_idx,(void **)mem )
);
BENCHMARK( bm_rs8_dec_xmm4 ,
	raid8_rs_ssse3x4_4fails_recov(N8,PAGE_SIZE,fail_idx,(void **)mem );
);
BENCHMARK( bm_rs8_dec_ymm1 ,
		(raid_rs_recov_data<ymm_x1,4>(K,PAGE_SIZE,fail_idx,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs8_dec_ymm2 ,
		(raid_rs_recov_data<ymm_x2,4>(K,PAGE_SIZE,fail_idx,(void**)mem,raid_rs))
);
BENCHMARK( bm_rs8_dec_ymm4 ,
		(raid_rs_recov_data<ymm_x4,4>(K,PAGE_SIZE,fail_idx,(void**)mem,raid_rs))
);
	}

	printf("\n====================================\n");
	printf("RS RAID8:\n\n");

	dump( bm_rs8_enc_xmm1 , "RS 8 xmm x 1 " , K , 4 );
	dump( bm_rs8_enc_xmm2 , "RS 8 xmm x 2 " , K , 4 );
	dump( bm_rs8_enc_xmm4 , "RS 8 xmm x 4 " , K , 4 );

	dump( bm_rs8_enc_ymm1 , "RS 8 ymm x 1 " , K , 4 );
	dump( bm_rs8_enc_ymm2 , "RS 8 ymm x 2 " , K , 4 );
	dump( bm_rs8_enc_ymm4 , "RS 8 ymm x 4 " , K , 4 );

	dump( bm_rs8_dec_xmm1 , "RS 8 decode xmm x 1 " , K , 4 );
	dump( bm_rs8_dec_xmm2 , "RS 8 decode xmm x 2 " , K , 4 );
	dump( bm_rs8_dec_xmm4 , "RS 8 decode xmm x 4 " , K , 4 );

	dump( bm_rs8_dec_ymm1 , "RS 8 decode ymm x 1 " , K , 4 );
	dump( bm_rs8_dec_ymm2 , "RS 8 decode ymm x 2 " , K , 4 );
	dump( bm_rs8_dec_ymm4 , "RS 8 decode ymm x 4 " , K , 4 );


#endif


/////////////////////////////////////////////////





	return 0;
}

