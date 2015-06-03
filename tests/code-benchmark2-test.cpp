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

#include <sys/time.h>
#include "benchmark.h"


#define CLOCK 3.4

#define TEST_RUN 1000
#define MEASURE_RUN 300


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

#define BM_N7
#define BM_RQ7_A2
#define BM_RQ7_SQRTA2
#define BM_RS7

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



uint64_t measure1( uint64_t &r_ms, void(*func)(int,size_t,void**), int disks , size_t size , void ** mem)
{
	struct timeval tval_before, tval_after, tval_result;
	int i = TEST_RUN;
	gettimeofday(&tval_before, NULL);
	for(;i>0;i--) {
		func(disks,size,mem);
	}
	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_result);

	r_ms = ((long int)tval_result.tv_sec)*1000000 + (long int)tval_result.tv_usec;
	return r_ms;
}


uint64_t measure_d1( uint64_t &r_ms, void(*func)(int,size_t,uint8_t *,void**), int disks , size_t size , uint8_t *f_idx, void ** mem)
{
	struct timeval tval_before, tval_after, tval_result;
	int i = TEST_RUN;
	gettimeofday(&tval_before, NULL);
	for(;i>0;i--) {
		func(disks,size,f_idx,mem);
	}
	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_result);

	r_ms = ((long int)tval_result.tv_sec)*1000000 + (long int)tval_result.tv_usec;
	return r_ms;
}

uint64_t measure2( uint64_t & r_ms, void(*func)(int,size_t,void**,const uint8_t **), int disks , size_t size , void ** mem, const uint8_t **rd_tab)
{
	struct timeval tval_before, tval_after, tval_result;
	int i = TEST_RUN;
	gettimeofday(&tval_before, NULL);
	for(;i>0;i--) {
		func(disks,size,mem,rd_tab);
	}
	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_result);

	r_ms = ((long int)tval_result.tv_sec)*1000000 + (long int)tval_result.tv_usec;
	return r_ms;
}

uint64_t measure_d2( uint64_t & r_ms, void(*func)(int,size_t,uint8_t *,void**,const uint8_t **), int disks , size_t size , uint8_t * f_idx,  void ** mem, const uint8_t **rd_tab)
{
	struct timeval tval_before, tval_after, tval_result;
	int i = TEST_RUN;
	gettimeofday(&tval_before, NULL);
	for(;i>0;i--) {
		func(disks,size,f_idx,mem,rd_tab);
	}
	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_result);

	r_ms = ((long int)tval_result.tv_sec)*1000000 + (long int)tval_result.tv_usec;
	return r_ms;
}

#include <vector>
#include <math.h>

struct mea
{
	std::vector<long> v;
	unsigned r,w;

	mea() : r(0),w(0) { clr(); }

	void clr() {v.clear();}
	void set( unsigned _r , unsigned _w ) { r=_r; w=_w;}

	void ins(uint64_t m) { v.push_back(m); }

	unsigned n() const { return v.size(); }

	double avg() const {
		double avg = 0;
		for(unsigned i=0;i<v.size();i++) avg += v[i];
		avg /= v.size();
		return avg;
	}
	double std_err() const {
		double a = avg();
		double std_err = 0;
		for(unsigned i=0;i<v.size();i++) std_err += ((double)v[i] - a)*((double)v[i] - a);
		std_err /= v.size();
		std_err = sqrt( std_err );
		return std_err;
	}

	void throughput() const {
		double r_size = r*PAGE_SIZE*TEST_RUN;
		double w_size = w*PAGE_SIZE*TEST_RUN;
		double av = avg();

		double cvt = (1000000.0/1024.0/1024.0/1024.0);

		printf("throughput r/w (GB/sec) (%d/%d): %lf/%lf\n" , r, w, r_size*cvt/av , w_size*cvt/av );

	}
	void say() const {
		double a = avg();
		double s = std_err();

		printf("avg: %lf  , std err: %lf ,  %f percent\n", a , s , (s/a)*100.0);
	}
};

void best( const mea& m1 , const mea& m2 , const mea& m3, const mea& m4 , const mea& m5 , const mea& m6 )
{
	const mea * bm = &m1;
	const mea * cpm = &m2;
	if( (0==bm->n()) ||  ((0!=cpm->n())&&(bm->avg()>cpm->avg())) ) bm = cpm;
	cpm = &m3;
	if( (0==bm->n()) ||  ((0!=cpm->n())&&(bm->avg()>cpm->avg())) ) bm = cpm;
	cpm = &m4;
	if( (0==bm->n()) ||  ((0!=cpm->n())&&(bm->avg()>cpm->avg())) ) bm = cpm;
	cpm = &m5;
	if( (0==bm->n()) ||  ((0!=cpm->n())&&(bm->avg()>cpm->avg())) ) bm = cpm;
	cpm = &m6;
	if( (0==bm->n()) ||  ((0!=cpm->n())&&(bm->avg()>cpm->avg())) ) bm = cpm;

	printf("best: \n");
	bm->throughput();
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


	uint64_t m_us = 0;
	void(*func1)(int,size_t,void**);
	void(*func2)(int,size_t,uint8_t*,void**);

	void(*func3)(int,size_t,void**,const uint8_t **);
	void(*func4)(int,size_t,uint8_t*,void**,const uint8_t **);

	mea m1,m2,m3,m4,m5,m6;



///////////////////////////////////////////////

#define MEA1( mm,m ) mm.clr(); \
	mm.set(K,m); \
	for(int i=0;i<MEASURE_RUN;i++) { \
		measure1( m_us , func1 , K+m,PAGE_SIZE,(void**)mem); \
		mm.ins(m_us); \
	} \
	mm.say(); \
	mm.throughput();



#define MEA2( mm,m ) mm.clr(); \
	mm.set(K,m); \
	for(int i=0;i<MEASURE_RUN;i++) { \
		measure_d1( m_us , func2 , K ,PAGE_SIZE, fail_idx ,(void**)mem); \
		mm.ins(m_us); \
	} \
	mm.say(); \
	mm.throughput();

#define MEA3( mm,m ) mm.clr(); \
	mm.set(K,m); \
	for(int i=0;i<MEASURE_RUN;i++) { \
		measure2( m_us , func3 , K+m ,PAGE_SIZE ,(void**)mem,raid_rs); \
		mm.ins(m_us); \
	} \
	mm.say(); \
	mm.throughput();

#define MEA4( mm,m ) mm.clr(); \
	mm.set(K,m); \
	for(int i=0;i<MEASURE_RUN;i++) { \
		measure_d2( m_us , func4 , K ,PAGE_SIZE, fail_idx ,(void**)mem,raid_rs); \
		mm.ins(m_us); \
	} \
	mm.say(); \
	mm.throughput();



#ifdef BM_R5

	func1 = &(raid5_gen_syndrome<__m128i>);
	printf("\n================\nRAID5 encode xmm_x1\n");
	MEA1( m1 , 1 );

	func1 = &(raid5_gen_syndrome<xmm_x2>);
	printf("\nRAID5 encode xmm_x2\n");
	MEA1( m2 , 1 );

	func1 = &(raid5_gen_syndrome<xmm_x4>);
	printf("\nRAID5 encode xmm_x4\n");
	MEA1( m3 , 1 );

	func1 = &(raid5_gen_syndrome<ymm_x1>);
	printf("\nRAID5 encode ymm_x1\n");
	MEA1( m4 , 1 );

	func1 = &(raid5_gen_syndrome<ymm_x2>);
	printf("\nRAID5 encode ymm_x2\n");
	MEA1( m5 , 1 );

	func1 = &(raid5_gen_syndrome<ymm_x4>);
	printf("\nRAID5 encode ymm_x4\n");
	MEA1( m6 , 1 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );

#endif



///////////////////////////////////////////

#ifdef BM_N6

	func1 = &(naive6_gen_syndrome<__m128i>);
	printf("\n================\nNaive6 encode xmm_x1\n");
	MEA1( m1, 2 );

	func1 = &(naive6_gen_syndrome<xmm_x2>);
	printf("\nNaive6 encode xmm_x2\n");
	MEA1( m2, 2 );

	func1 = &(naive6_gen_syndrome<xmm_x4>);
	printf("\nnaive6 encode xmm_x4\n");
	MEA1( m3, 2 );

	func1 = &(naive6_gen_syndrome<ymm_x1>);
	printf("\nnaive6 encode ymm_x1\n");
	MEA1( m4, 2 );

	func1 = &(naive6_gen_syndrome<ymm_x2>);
	printf("\nnaive6 encode ymm_x2\n");
	MEA1( m5, 2 );

	func1 = &(naive6_gen_syndrome<ymm_x4>);
	printf("\nnaive6 encode ymm_x4\n");
	MEA1( m6, 2 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );

#endif

/////////////////////////////////////////////////

#ifdef BM_R6

	func1 = &(raid6_gen_syndrome<__m128i>);
	printf("\n================\nRAID6 encode xmm_x1\n");
	MEA1( m1, 2 );

	func1 = &(raid6_gen_syndrome<xmm_x2>);
	printf("\nRAID6 encode xmm_x2\n");
	MEA1( m2, 2 );

	func1 = &(raid6_gen_syndrome<xmm_x4>);
	printf("\nraid6 encode xmm_x4\n");
	MEA1( m3, 2 );

	func1 = &(raid6_gen_syndrome<ymm_x1>);
	printf("\nraid6 encode ymm_x1\n");
	MEA1( m4, 2 );

	func1 = &(raid6_gen_syndrome<ymm_x2>);
	printf("\nraid6 encode ymm_x2\n");
	MEA1( m5, 2 );

	func1 = &(raid6_gen_syndrome<ymm_x4>);
	printf("\nraid6 encode ymm_x4\n");
	MEA1( m6, 2 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );



	func2 = &(raid6_recov_data<__m128i>);
	printf("\n================\nRAID6 decode xmm_x1\n");
	MEA2( m1, 2 );

	func2 = &(raid6_recov_data<xmm_x2>);
	printf("\nRAID6 decode xmm_x2\n");
	MEA2( m2, 2 );

	func2 = &(raid6_recov_data<xmm_x4>);
	printf("\nraid6 decode xmm_x4\n");
	MEA2( m3, 2 );

	func2 = &(raid6_recov_data<ymm_x1>);
	printf("\nraid6 decode ymm_x1\n");
	MEA2( m4, 2 );

	func2 = &(raid6_recov_data<ymm_x2>);
	printf("\nraid6 decode ymm_x2\n");
	MEA2( m5, 2 );

	func2 = &(raid6_recov_data<ymm_x4>);
	printf("\nraid6 decode ymm_x4\n");
	MEA2( m6, 2 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );

#endif


///////////////////////////////////////////////


///////////////////////////////////////////////

#ifdef BM_RS6

	func3 = &(gen_syndrome<__m128i,2>);
	printf("\n================\nRS RAID6 encode xmm_x1\n");
	MEA3( m1, 2 );

	func3 = &(gen_syndrome<xmm_x2,2>);
	printf("\nRS RAID6 encode xmm_x2\n");
	MEA3( m2, 2 );

	func3 = &(gen_syndrome<xmm_x4,2>);
	printf("\nRS raid6 encode xmm_x4\n");
	MEA3( m3, 2 );

	func3 = &(gen_syndrome<ymm_x1,2>);
	printf("\nRS raid6 encode ymm_x1\n");
	MEA3( m4, 2 );

	func3 = &(gen_syndrome<ymm_x2,2>);
	printf("\nRS raid6 encode ymm_x2\n");
	MEA3( m5, 2 );

	func3 = &(gen_syndrome<ymm_x4,2>);
	printf("\nRS raid6 encode ymm_x4\n");
	MEA3( m6, 2 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );



	func4 = &(raid_rs_recov_data<__m128i,2>);
	printf("\n================\nRS RAID6 decode xmm_x1\n");
	MEA4( m1, 2 );

	func4 = &(raid_rs_recov_data<xmm_x2,2>);
	printf("\nRS RAID6 decode xmm_x2\n");
	MEA4( m2, 2 );

	func4 = &(raid_rs_recov_data<xmm_x4,2>);
	printf("\nRS raid6 decode xmm_x4\n");
	MEA4( m3, 2 );

	func4 = &(raid_rs_recov_data<ymm_x1,2>);
	printf("\nRS raid6 decode ymm_x1\n");
	MEA4( m4, 2 );

	func4 = &(raid_rs_recov_data<ymm_x2,2>);
	printf("\nRS raid6 decode ymm_x2\n");
	MEA4( m5, 2 );

	func4 = &(raid_rs_recov_data<ymm_x4,2>);
	printf("\nRS raid6 decode ymm_x4\n");
	MEA4( m6, 2 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );

#endif


/////////////////////////////////////////////////


#ifdef BM_N7


	func1 = &(naive7_gen_syndrome<__m128i>);
	printf("\n================\nnaive7 encode xmm_x1\n");
	MEA1( m1, 3 );

	func1 = &(naive7_gen_syndrome<xmm_x2>);
	printf("\nnaive7 encode xmm_x2\n");
	MEA1( m2, 3 );

	func1 = &(naive7_gen_syndrome<xmm_x4>);
	printf("\nnaive7 encode xmm_x4\n");
	MEA1( m3, 3 );

	func1 = &(naive7_gen_syndrome<ymm_x1>);
	printf("\nnaive7 encode ymm_x1\n");
	MEA1( m4, 3 );

	func1 = &(naive7_gen_syndrome<ymm_x2>);
	printf("\nnaive7 encode ymm_x2\n");
	MEA1( m5, 3 );

	func1 = &(naive7_gen_syndrome<ymm_x4>);
	printf("\nnaive7 encode ymm_x4\n");
	MEA1( m6, 3 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );


#endif


////////////////////////////////////////////////

#ifdef BM_RQ7_A2


	func1 = &(raidq7_a2_gen_syndrome<__m128i>);
	printf("\n================\nraidq7_a2 encode xmm_x1\n");
	MEA1( m1, 3 );

	func1 = &(raidq7_a2_gen_syndrome<xmm_x2>);
	printf("\nraidq7_a2 encode xmm_x2\n");
	MEA1( m2, 3 );

	func1 = &(raidq7_a2_gen_syndrome<xmm_x4>);
	printf("\nraidq7_a2 encode xmm_x4\n");
	MEA1( m3, 3 );

	func1 = &(raidq7_a2_gen_syndrome<ymm_x1>);
	printf("\nraidq7_a2 encode ymm_x1\n");
	MEA1( m4, 3 );

	func1 = &(raidq7_a2_gen_syndrome<ymm_x2>);
	printf("\nraidq7_a2 encode ymm_x2\n");
	MEA1( m5, 3 );

	func1 = &(raidq7_a2_gen_syndrome<ymm_x4>);
	printf("\nraidq7_a2 encode ymm_x4\n");
	MEA1( m6, 3 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );



	func2 = &(raidq7_a2_recov_data<__m128i>);
	printf("\n================\nraidq7_a2 decode xmm_x1\n");
	MEA2( m1, 3 );

	func2 = &(raidq7_a2_recov_data<xmm_x2>);
	printf("\nraidq7_a2 decode xmm_x2\n");
	MEA2( m2, 3 );

	func2 = &(raidq7_a2_recov_data<xmm_x4>);
	printf("\nraidq7_a2 decode xmm_x4\n");
	MEA2( m3, 3 );

	func2 = &(raidq7_a2_recov_data<ymm_x1>);
	printf("\nraidq7_a2 decode ymm_x1\n");
	MEA2( m4, 3 );

	func2 = &(raidq7_a2_recov_data<ymm_x2>);
	printf("\nraidq7_a2 decode ymm_x2\n");
	MEA2( m5, 3 );

	func2 = &(raidq7_a2_recov_data<ymm_x4>);
	printf("\nraidq7_a2 decode ymm_x4\n");
	MEA2( m6, 3 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );

#endif

/////////////////////////////////////////////


/////////////////////////////////////////////


#ifdef BM_RQ7_SQRTA2



	func1 = &(raidq7_sqrta_gen_syndrome2<__m128i>);
	printf("\n================\nraidq7_sqrta encode xmm_x1\n");
	MEA1( m1, 3 );

	func1 = &(raidq7_sqrta_gen_syndrome2<xmm_x2>);
	printf("\nraidq7_sqrta encode xmm_x2\n");
	MEA1( m2, 3 );

	func1 = &(raidq7_sqrta_gen_syndrome2<xmm_x4>);
	printf("\nraidq7_sqrta encode xmm_x4\n");
	MEA1( m3, 3 );

	func1 = &(raidq7_sqrta_gen_syndrome2<ymm_x1>);
	printf("\nraidq7_sqrta encode ymm_x1\n");
	MEA1( m4, 3 );

	func1 = &(raidq7_sqrta_gen_syndrome2<ymm_x2>);
	printf("\nraidq7_sqrta encode ymm_x2\n");
	MEA1( m5, 3 );

	func1 = &(raidq7_sqrta_gen_syndrome2<ymm_x4>);
	printf("\nraidq7_sqrta encode ymm_x4\n");
	MEA1( m6, 3 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );



	func2 = &(raidq7_sqrta_recov_data2<__m128i>);
	printf("\n================\nraidq7_sqrta decode xmm_x1\n");
	MEA2( m1, 3 );

	func2 = &(raidq7_sqrta_recov_data2<xmm_x2>);
	printf("\nraidq7_sqrta decode xmm_x2\n");
	MEA2( m2, 3 );

	func2 = &(raidq7_sqrta_recov_data2<xmm_x4>);
	printf("\nraidq7_sqrta decode xmm_x4\n");
	MEA2( m3, 3 );

	func2 = &(raidq7_sqrta_recov_data2<ymm_x1>);
	printf("\nraidq7_sqrta decode ymm_x1\n");
	MEA2( m4, 3 );

	func2 = &(raidq7_sqrta_recov_data2<ymm_x2>);
	printf("\nraidq7_sqrta decode ymm_x2\n");
	MEA2( m5, 3 );

	func2 = &(raidq7_sqrta_recov_data2<ymm_x4>);
	printf("\nraidq7_sqrta decode ymm_x4\n");
	MEA2( m6, 3 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );


#endif



/////////////////////////////////////////////


#ifdef BM_RS7


	func3 = &(gen_syndrome<__m128i,3>);
	printf("\n================\nRS RAID7 encode xmm_x1\n");
	MEA3( m1, 3 );

	func3 = &(gen_syndrome<xmm_x2,3>);
	printf("\nRS RAID7 encode xmm_x2\n");
	MEA3( m2, 3 );

	func3 = &(gen_syndrome<xmm_x4,3>);
	printf("\nRS raid7 encode xmm_x4\n");
	MEA3( m3, 3 );

	func3 = &(gen_syndrome<ymm_x1,3>);
	printf("\nRS raid7 encode ymm_x1\n");
	MEA3( m4, 3 );

	func3 = &(gen_syndrome<ymm_x2,3>);
	printf("\nRS raid7 encode ymm_x2\n");
	MEA3( m5, 3 );

	func3 = &(gen_syndrome<ymm_x4,3>);
	printf("\nRS raid7 encode ymm_x4\n");
	MEA3( m6, 3 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );



	func4 = &(raid_rs_recov_data<__m128i,3>);
	printf("\n================\nRS RAID7 decode xmm_x1\n");
	MEA4( m1, 3 );

	func4 = &(raid_rs_recov_data<xmm_x2,3>);
	printf("\nRS RAID7 decode xmm_x2\n");
	MEA4( m2, 3 );

	func4 = &(raid_rs_recov_data<xmm_x4,3>);
	printf("\nRS raid7 decode xmm_x4\n");
	MEA4( m3, 3 );

	func4 = &(raid_rs_recov_data<ymm_x1,3>);
	printf("\nRS raid7 decode ymm_x1\n");
	MEA4( m4, 3 );

	func4 = &(raid_rs_recov_data<ymm_x2,3>);
	printf("\nRS raid7 decode ymm_x2\n");
	MEA4( m5, 3 );

	func4 = &(raid_rs_recov_data<ymm_x4,3>);
	printf("\nRS raid7 decode ymm_x4\n");
	MEA4( m6, 3 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );



#endif



/////////////////////////////////////////////////


#ifdef BM_N8

	func1 = &(naive8_gen_syndrome<__m128i>);
	printf("\n================\nnaive8 encode xmm_x1\n");
	MEA1( m1, 4 );

	func1 = &(naive8_gen_syndrome<xmm_x2>);
	printf("\nnaive8 encode xmm_x2\n");
	MEA1( m2, 4 );

	func1 = &(naive8_gen_syndrome<xmm_x4>);
	printf("\nnaive8 encode xmm_x4\n");
	MEA1( m3, 4 );

	func1 = &(naive8_gen_syndrome<ymm_x1>);
	printf("\nnaive8 encode ymm_x1\n");
	MEA1( m4, 4 );

	func1 = &(naive8_gen_syndrome<ymm_x2>);
	printf("\nnaive8 encode ymm_x2\n");
	MEA1( m5, 4 );

	func1 = &(naive8_gen_syndrome<ymm_x4>);
	printf("\nnaive8 encode ymm_x4\n");
	MEA1( m6, 4 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );


#endif



/////////////////////////////////////////////


#ifdef BM_RQ8_X

	printf("\n================\n");

	m1.clr();

	func1 = &(raidq8_X_gen_syndrome<xmm_x2>);
	printf("\nraidq8_X encode xmm_x2\n");
	MEA1( m2, 4 );

	func1 = &(raidq8_X_gen_syndrome<xmm_x4>);
	printf("\nraidq8_X encode xmm_x4\n");
	MEA1( m3, 4 );

	m4.clr();

	func1 = &(raidq8_X_gen_syndrome<ymm_x2>);
	printf("\nraidq8_X encode ymm_x2\n");
	MEA1( m5, 4 );

	func1 = &(raidq8_X_gen_syndrome<ymm_x4>);
	printf("\nraidq8_X encode ymm_x4\n");
	MEA1( m6, 4 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );

	printf("\n================\n");


	m1.clr();

	func2 = &(raidq8_X_recov_data<xmm_x2>);
	printf("\nraidq8_X decode xmm_x2\n");
	MEA2( m2, 4 );

	func2 = &(raidq8_X_recov_data<xmm_x4>);
	printf("\nraidq8_X decode xmm_x4\n");
	MEA2( m3, 4 );

	m4.clr();

	func2 = &(raidq8_X_recov_data<ymm_x2>);
	printf("\nraidq8_X decode ymm_x2\n");
	MEA2( m5, 4 );

	func2 = &(raidq8_X_recov_data<ymm_x4>);
	printf("\nraidq8_X decode ymm_x4\n");
	MEA2( m6, 4 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );


#endif




/////////////////////////////////////////////

#ifdef BM_RQ8_A2XP1

	printf("\n================\n");


	m1.clr();

	func1 = &(raidq8_a2Xp1_gen_syndrome<xmm_x2>);
	printf("\nraidq8_a2Xp1 encode xmm_x2\n");
	MEA1( m2, 4 );

	func1 = &(raidq8_a2Xp1_gen_syndrome<xmm_x4>);
	printf("\nraidq8_a2Xp1 encode xmm_x4\n");
	MEA1( m3, 4 );

	m4.clr();

	func1 = &(raidq8_a2Xp1_gen_syndrome<ymm_x2>);
	printf("\nraidq8_a2Xp1 encode ymm_x2\n");
	MEA1( m5, 4 );

	func1 = &(raidq8_a2Xp1_gen_syndrome<ymm_x4>);
	printf("\nraidq8_a2Xp1 encode ymm_x4\n");
	MEA1( m6, 4 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );

	printf("\n================\n");


	m1.clr();

	func2 = &(raidq8_a2Xp1_recov_data<xmm_x2>);
	printf("\nraidq8_a2Xp1 decode xmm_x2\n");
	MEA2( m2, 4 );

	func2 = &(raidq8_a2Xp1_recov_data<xmm_x4>);
	printf("\nraidq8_a2Xp1 decode xmm_x4\n");
	MEA2( m3, 4 );

	m4.clr();

	func2 = &(raidq8_a2Xp1_recov_data<ymm_x2>);
	printf("\nraidq8_a2Xp1 decode ymm_x2\n");
	MEA2( m5, 4 );

	func2 = &(raidq8_a2Xp1_recov_data<ymm_x4>);
	printf("\nraidq8_a2Xp1 decode ymm_x4\n");
	MEA2( m6, 4 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );


#endif



/////////////////////////////////////////////


#ifdef BM_RQ8_A141X

	printf("\n================\n");


	m1.clr();

	func1 = &(raidq8_a141X_gen_syndrome<xmm_x2>);
	printf("\nraidq8_a141X encode xmm_x2\n");
	MEA1( m2, 4 );

	func1 = &(raidq8_a141X_gen_syndrome<xmm_x4>);
	printf("\nraidq8_a141X encode xmm_x4\n");
	MEA1( m3, 4 );

	m4.clr();

	func1 = &(raidq8_a141X_gen_syndrome<ymm_x2>);
	printf("\nraidq8_a141X encode ymm_x2\n");
	MEA1( m5, 4 );

	func1 = &(raidq8_a141X_gen_syndrome<ymm_x4>);
	printf("\nraidq8_a141X encode ymm_x4\n");
	MEA1( m6, 4 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );

	printf("\n================\n");


	m1.clr();

	func2 = &(raidq8_a141X_recov_data<xmm_x2>);
	printf("\nraidq8_a141X decode xmm_x2\n");
	MEA2( m2, 4 );

	func2 = &(raidq8_a141X_recov_data<xmm_x4>);
	printf("\nraidq8_a141X decode xmm_x4\n");
	MEA2( m3, 4 );

	m4.clr();

	func2 = &(raidq8_a141X_recov_data<ymm_x2>);
	printf("\nraidq8_a141X decode ymm_x2\n");
	MEA2( m5, 4 );

	func2 = &(raidq8_a141X_recov_data<ymm_x4>);
	printf("\nraidq8_a141X decode ymm_x4\n");
	MEA2( m6, 4 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );


#endif



/////////////////////////////////////////////


#ifdef BM_A186XPA6


	printf("\n================\n");


	m1.clr();

	func1 = &(raidq8_a186Xpa6_gen_syndrome<xmm_x2>);
	printf("\nraidq8_a186Xpa6 encode xmm_x2\n");
	MEA1( m2, 4 );

	func1 = &(raidq8_a186Xpa6_gen_syndrome<xmm_x4>);
	printf("\nraidq8_a186Xpa6 encode xmm_x4\n");
	MEA1( m3, 4 );

	m4.clr();

	func1 = &(raidq8_a186Xpa6_gen_syndrome<ymm_x2>);
	printf("\nraidq8_a186Xpa6 encode ymm_x2\n");
	MEA1( m5, 4 );

	func1 = &(raidq8_a186Xpa6_gen_syndrome<ymm_x4>);
	printf("\nraidq8_a186Xpa6 encode ymm_x4\n");
	MEA1( m6, 4 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );

	printf("\n================\n");


	m1.clr();

	func2 = &(raidq8_a186Xpa6_recov_data<xmm_x2>);
	printf("\nraidq8_a186Xpa6 decode xmm_x2\n");
	MEA2( m2, 4 );

	func2 = &(raidq8_a186Xpa6_recov_data<xmm_x4>);
	printf("\nraidq8_a186Xpa6 decode xmm_x4\n");
	MEA2( m3, 4 );

	m4.clr();

	func2 = &(raidq8_a186Xpa6_recov_data<ymm_x2>);
	printf("\nraidq8_a186Xpa6 decode ymm_x2\n");
	MEA2( m5, 4 );

	func2 = &(raidq8_a186Xpa6_recov_data<ymm_x4>);
	printf("\nraidq8_a186Xpa6 decode ymm_x4\n");
	MEA2( m6, 4 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );



#endif



/////////////////////////////////////////////


#ifdef BM_RS8



	func3 = &(gen_syndrome<__m128i,4>);
	printf("\n================\nRS raid8 encode xmm_x1\n");
	MEA3( m1, 4 );

	func3 = &(gen_syndrome<xmm_x2,4>);
	printf("\nRS raid8 encode xmm_x2\n");
	MEA3( m2, 4 );

	func3 = &(gen_syndrome<xmm_x4,4>);
	printf("\nRS raid8 encode xmm_x4\n");
	MEA3( m3, 4 );

	func3 = &(gen_syndrome<ymm_x1,4>);
	printf("\nRS raid8 encode ymm_x1\n");
	MEA3( m4, 4 );

	func3 = &(gen_syndrome<ymm_x2,4>);
	printf("\nRS raid8 encode ymm_x2\n");
	MEA3( m5, 4 );

	func3 = &(gen_syndrome<ymm_x4,4>);
	printf("\nRS raid8 encode ymm_x4\n");
	MEA3( m6, 4 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );



	func4 = &(raid_rs_recov_data<__m128i,4>);
	printf("\n================\nRS raid8 decode xmm_x1\n");
	MEA4( m1, 4 );

	func4 = &(raid_rs_recov_data<xmm_x2,4>);
	printf("\nRS raid8 decode xmm_x2\n");
	MEA4( m2, 4 );

	func4 = &(raid_rs_recov_data<xmm_x4,4>);
	printf("\nRS raid8 decode xmm_x4\n");
	MEA4( m3, 4 );

	func4 = &(raid_rs_recov_data<ymm_x1,4>);
	printf("\nRS raid8 decode ymm_x1\n");
	MEA4( m4, 4 );

	func4 = &(raid_rs_recov_data<ymm_x2,4>);
	printf("\nRS raid8 decode ymm_x2\n");
	MEA4( m5, 4 );

	func4 = &(raid_rs_recov_data<ymm_x4,4>);
	printf("\nRS raid8 decode ymm_x4\n");
	MEA4( m6, 4 );

	best( m1 , m2 , m3 , m4 , m5 , m6 );


#endif


/////////////////////////////////////////////////





	return 0;
}

