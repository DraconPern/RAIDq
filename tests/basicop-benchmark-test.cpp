#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include <assert.h>

#include "stdint.h"

#include "raid_table.h"
#include "rs_table.h"

#include "gf256.h"
#include "gf256_xmm.h"
#include "gf65536.h"

#include "benchmark.h"

/// GHz
#define CLOCK 3.4

#define TEST_RUN 10000

#define PAGE_SIZE 4096

#define K 128


//#define BM_XOR
//#define BM_A
#define BM_A2
#define BM_SQRTA
//#define BM_M


//const char raid6_empty_zero_page[PAGE_SIZE] __attribute__ ((aligned (256))) = {0};




void randomize_mem( uint8_t * mem , unsigned n_pages )
{
	unsigned i,j;
	for(i=0;i<n_pages;i++) {
		for(j=0;j<PAGE_SIZE;j++) mem[i*PAGE_SIZE+j] = rand()&0xff;
	}
}



void dump(struct benchmark & bm , const char * str , int k )
{
	char buf[1024];
	printf("%s , (%d x %d)\n",str,k,PAGE_SIZE );
	bm_dump(buf, sizeof(buf), &bm);
	printf("%s : %s\n" , str , buf);

	double r_datasize = k*PAGE_SIZE;
	printf("data size: %f , throughput: %f GByte/sec\n", r_datasize , r_datasize*TEST_RUN*CLOCK/bm.acc );
	printf("%f clocks for one page\n",bm.acc/k/TEST_RUN);
	printf("\n");
}




template <typename sto_t>
void bench_xor(  void * mem )
{
	sto_t * ptr = (sto_t *) mem;
	unsigned time = K*PAGE_SIZE/sizeof(sto_t);
	time --;
	sto_t * prev = ptr;
	ptr++;
	for( ; time > 0; time-- ){
		prev[0] ^= ptr[0];
		prev = ptr;
		ptr++;
	}
}

template <typename sto_t>
void bench_mul( void * mem )
{
	sto_t * ptr = (sto_t *) mem;
	union {
	unsigned time;
	uint8_t b;
	};
	time = K*PAGE_SIZE/sizeof(sto_t);
	for( ; time > 0; time-- ){
		*ptr = gfv_mul( *ptr , b );
		ptr++;
	}
}

template <typename sto_t>
void bench_mula( void * mem )
{
	sto_t * ptr = (sto_t *) mem;
	unsigned time = K*PAGE_SIZE/sizeof(sto_t);
	for( ; time > 0; time-- ){
		mul_alpha( *ptr );
		ptr++;
	}
}

template <typename sto_t>
void bench_mula2(  void * mem )
{
	sto_t * ptr = (sto_t *) mem;
	unsigned time = K*PAGE_SIZE/sizeof(sto_t);
	for( ; time > 0; time-- ){
		mul_alpha2( *ptr );
		ptr++;
	}
}

template <typename sto_t>
void bench_mulsqrta(  void * mem )
{
	sto_t * ptr = (sto_t *) mem;
	unsigned time = K*PAGE_SIZE/sizeof(sto_t);
	for( ; time > 0; time-- ){
		mul_sqrt_alpha( *ptr );
		ptr++;
	}
}





int main()
{

	unsigned i;

	uint8_t * mem;
	//uint8_t * mem2[N8];

	srand(23);

	if( 0 != posix_memalign( (void **) &mem , 32, PAGE_SIZE*K) ) { printf("mem fail\n"); exit(-1); }

	randomize_mem( mem , K );

	/* warming up */
	for(i=0;i<K-1;i++) {
		for(unsigned j=0;j<PAGE_SIZE;j++) {
			mem[(i+1)*PAGE_SIZE+j] ^= mem[i*PAGE_SIZE+j];
		}
	}


///////////////////////////////////////////////

	printf("================================\n");
	printf("     benchmarker for basic operations \n");
	printf("================================\n\n");



///////////////////////////////////////////////

#ifdef BM_XOR

	struct benchmark bm_xor_u64;
	struct benchmark bm_xor_xmm1;
	struct benchmark bm_xor_xmm2;
	struct benchmark bm_xor_xmm4;
	struct benchmark bm_xor_ymm1;
	struct benchmark bm_xor_ymm2;
	struct benchmark bm_xor_ymm4;

	bm_init(&bm_xor_u64);
	bm_init(&bm_xor_xmm1);
	bm_init(&bm_xor_xmm2);
	bm_init(&bm_xor_xmm4);
	bm_init(&bm_xor_ymm1);
	bm_init(&bm_xor_ymm2);
	bm_init(&bm_xor_ymm4);

	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_xor_u64 ,
		bench_xor<uint64_t>((void*)mem)
);
BENCHMARK( bm_xor_xmm1 ,
		bench_xor<__m128i>((void*)mem)
);
BENCHMARK( bm_xor_xmm2 ,
		bench_xor<xmm_x2>((void*)mem)
);
BENCHMARK( bm_xor_xmm4 ,
		bench_xor<xmm_x4>((void*)mem)
);
BENCHMARK( bm_xor_ymm1 ,
		bench_xor<ymm_x1>((void*)mem)
);
BENCHMARK( bm_xor_ymm2 ,
		bench_xor<ymm_x2>((void*)mem)
);
BENCHMARK( bm_xor_ymm4 ,
		bench_xor<ymm_x4>((void*)mem)
);
	}
	printf("\n====================================\n");
	printf("XOR:\n\n");

	dump( bm_xor_u64 , "XOR u64 " , K );
	dump( bm_xor_xmm1 , "XOR xmm1 " , K );
	dump( bm_xor_xmm2 , "XOR xmm2 " , K );
	dump( bm_xor_xmm4 , "XOR xmm4 " , K );
	dump( bm_xor_ymm1 , "XOR ymm1 " , K );
	dump( bm_xor_ymm2 , "XOR ymm2 " , K );
	dump( bm_xor_ymm4 , "XOR ymm4 " , K );

#endif

///////////////////////////////////////////

#ifdef BM_A


	struct benchmark bm_a_u64;
	struct benchmark bm_a_xmm1;
	struct benchmark bm_a_xmm2;
	struct benchmark bm_a_xmm4;
	struct benchmark bm_a_ymm1;
	struct benchmark bm_a_ymm2;
	struct benchmark bm_a_ymm4;

	bm_init(&bm_a_u64);
	bm_init(&bm_a_xmm1);
	bm_init(&bm_a_xmm2);
	bm_init(&bm_a_xmm4);
	bm_init(&bm_a_ymm1);
	bm_init(&bm_a_ymm2);
	bm_init(&bm_a_ymm4);

	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_a_u64 ,
		bench_mula<uint64_t>((void*)mem)
);
BENCHMARK( bm_a_xmm1 ,
		bench_mula<__m128i>((void*)mem)
);
BENCHMARK( bm_a_xmm2 ,
		bench_mula<xmm_x2>((void*)mem)
);
BENCHMARK( bm_a_xmm4 ,
		bench_mula<xmm_x4>((void*)mem)
);
BENCHMARK( bm_a_ymm1 ,
		bench_mula<ymm_x1>((void*)mem)
);
BENCHMARK( bm_a_ymm2 ,
		bench_mula<ymm_x2>((void*)mem)
);
BENCHMARK( bm_a_ymm4 ,
		bench_mula<ymm_x4>((void*)mem)
);
	}
	printf("\n====================================\n");
	printf("mul a:\n\n");

	dump( bm_a_u64 , "mula u64 " , K );
	dump( bm_a_xmm1 , "mula xmm1 " , K );
	dump( bm_a_xmm2 , "mula xmm2 " , K );
	dump( bm_a_xmm4 , "mula xmm4 " , K );
	dump( bm_a_ymm1 , "mula ymm1 " , K );
	dump( bm_a_ymm2 , "mula ymm2 " , K );
	dump( bm_a_ymm4 , "mula ymm4 " , K );

#endif


/////////////////////////////////////////////////

#ifdef BM_A2

	struct benchmark bm_a2_xmm1;
	struct benchmark bm_a2_xmm2;
	struct benchmark bm_a2_xmm4;
	struct benchmark bm_a2_ymm1;
	struct benchmark bm_a2_ymm2;
	struct benchmark bm_a2_ymm4;

	bm_init(&bm_a2_xmm1);
	bm_init(&bm_a2_xmm2);
	bm_init(&bm_a2_xmm4);
	bm_init(&bm_a2_ymm1);
	bm_init(&bm_a2_ymm2);
	bm_init(&bm_a2_ymm4);

	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_a2_xmm1 ,
		bench_mula2<__m128i>((void*)mem)
);
BENCHMARK( bm_a2_xmm2 ,
		bench_mula2<xmm_x2>((void*)mem)
);
BENCHMARK( bm_a2_xmm4 ,
		bench_mula2<xmm_x4>((void*)mem)
);
BENCHMARK( bm_a2_ymm1 ,
		bench_mula2<ymm_x1>((void*)mem)
);
BENCHMARK( bm_a2_ymm2 ,
		bench_mula2<ymm_x2>((void*)mem)
);
BENCHMARK( bm_a2_ymm4 ,
		bench_mula2<ymm_x4>((void*)mem)
);
	}
	printf("\n====================================\n");
	printf("mul a2:\n\n");

	dump( bm_a2_xmm1 , "mula2 xmm1 " , K );
	dump( bm_a2_xmm2 , "mula2 xmm2 " , K );
	dump( bm_a2_xmm4 , "mula2 xmm4 " , K );
	dump( bm_a2_ymm1 , "mula2 ymm1 " , K );
	dump( bm_a2_ymm2 , "mula2 ymm2 " , K );
	dump( bm_a2_ymm4 , "mula2 ymm4 " , K );

#endif

/////////////////////////////////////////////////


#ifdef BM_SQRTA


	struct benchmark bm_sqrta_xmm1;
	struct benchmark bm_sqrta_xmm2;
	struct benchmark bm_sqrta_xmm4;
	struct benchmark bm_sqrta_ymm1;
	struct benchmark bm_sqrta_ymm2;
	struct benchmark bm_sqrta_ymm4;

	bm_init(&bm_sqrta_xmm1);
	bm_init(&bm_sqrta_xmm2);
	bm_init(&bm_sqrta_xmm4);
	bm_init(&bm_sqrta_ymm1);
	bm_init(&bm_sqrta_ymm2);
	bm_init(&bm_sqrta_ymm4);

	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_sqrta_xmm1 ,
		bench_mulsqrta<__m128i>((void*)mem)
);
BENCHMARK( bm_sqrta_xmm2 ,
		bench_mulsqrta<xmm_x2>((void*)mem)
);
BENCHMARK( bm_sqrta_xmm4 ,
		bench_mulsqrta<xmm_x4>((void*)mem)
);
BENCHMARK( bm_sqrta_ymm1 ,
		bench_mulsqrta<ymm_x1>((void*)mem)
);
BENCHMARK( bm_sqrta_ymm2 ,
		bench_mulsqrta<ymm_x2>((void*)mem)
);
BENCHMARK( bm_sqrta_ymm4 ,
		bench_mulsqrta<ymm_x4>((void*)mem)
);
	}
	printf("\n====================================\n");
	printf("mul sqrta:\n\n");

	dump( bm_sqrta_xmm1 , "mul sqrta xmm1 " , K );
	dump( bm_sqrta_xmm2 , "mul sqrta xmm2 " , K );
	dump( bm_sqrta_xmm4 , "mul sqrta xmm4 " , K );
	dump( bm_sqrta_ymm1 , "mul sqrta ymm1 " , K );
	dump( bm_sqrta_ymm2 , "mul sqrta ymm2 " , K );
	dump( bm_sqrta_ymm4 , "mul sqrta ymm4 " , K );

#endif



/////////////////////////////////////////////////


#ifdef BM_M

	struct benchmark bm_m_xmm1;
	struct benchmark bm_m_xmm2;
	struct benchmark bm_m_xmm4;
	struct benchmark bm_m_ymm1;
	struct benchmark bm_m_ymm2;
	struct benchmark bm_m_ymm4;

	bm_init(&bm_m_xmm1);
	bm_init(&bm_m_xmm2);
	bm_init(&bm_m_xmm4);
	bm_init(&bm_m_ymm1);
	bm_init(&bm_m_ymm2);
	bm_init(&bm_m_ymm4);

	for(i=0;i<TEST_RUN;i++) {
BENCHMARK( bm_m_xmm1 ,
		bench_mul<__m128i>((void*)mem)
);
BENCHMARK( bm_m_xmm2 ,
		bench_mul<xmm_x2>((void*)mem)
);
BENCHMARK( bm_m_xmm4 ,
		bench_mul<xmm_x4>((void*)mem)
);
BENCHMARK( bm_m_ymm1 ,
		bench_mul<ymm_x1>((void*)mem)
);
BENCHMARK( bm_m_ymm2 ,
		bench_mul<ymm_x2>((void*)mem)
);
BENCHMARK( bm_m_ymm4 ,
		bench_mul<ymm_x4>((void*)mem)
);
	}
	printf("\n====================================\n");
	printf("mul :\n\n");

	dump( bm_m_xmm1 , "mul xmm1 " , K );
	dump( bm_m_xmm2 , "mul xmm2 " , K );
	dump( bm_m_xmm4 , "mul xmm4 " , K );
	dump( bm_m_ymm1 , "mul ymm1 " , K );
	dump( bm_m_ymm2 , "mul ymm2 " , K );
	dump( bm_m_ymm4 , "mul ymm4 " , K );

#endif

/////////////////////////////////////////////////


	return 0;
}

