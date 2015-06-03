#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include <assert.h>

#include "stdint.h"

#include "raid_table.h"
#include "raidq.h"

#include "raidq.hpp"

#include "error_decoder_q.hpp"

#include <sys/time.h>



#define K 64
#define N (K+4)


#define TEST_RUN 1000
#define MEASURE_RUN 300

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


void insert_ei_ymmx2( uint8_t ei[2]  , unsigned pi, unsigned ith , uint8_t ** mem )
{
	unsigned n_ymmx2 = ith >> 5;
	unsigned idx_in_ymmx2 = ith & 0x1f;

	mem[pi][n_ymmx2*32*2 + idx_in_ymmx2] = ei[0];
	mem[pi][n_ymmx2*32*2 + 32 + idx_in_ymmx2] = ei[1];
}

void xor_ei_ymmx2( uint8_t ei[2]  , unsigned pi, unsigned ith , uint8_t ** mem )
{
	unsigned n_ymmx2 = ith >> 5;
	unsigned idx_in_ymmx2 = ith & 0x1f;

	mem[pi][n_ymmx2*32*2 + idx_in_ymmx2] ^= ei[0];
	mem[pi][n_ymmx2*32*2 + 32 + idx_in_ymmx2] ^= ei[1];
}

void get_synds_ymmx2( uint8_t s[8] , unsigned ith , uint8_t ** mem )
{
	unsigned n_ymmx2 = ith >> 5;
	unsigned idx_in_ymmx2 = ith & 0x1f;

	s[0] = mem[0][n_ymmx2*32*2 + idx_in_ymmx2];
	s[1] = mem[0][n_ymmx2*32*2 + 32 + idx_in_ymmx2];
	s[2] = mem[1][n_ymmx2*32*2 + idx_in_ymmx2];
	s[3] = mem[1][n_ymmx2*32*2 + 32 + idx_in_ymmx2];
	s[4] = mem[2][n_ymmx2*32*2 + idx_in_ymmx2];
	s[5] = mem[2][n_ymmx2*32*2 + 32 + idx_in_ymmx2];
	s[6] = mem[3][n_ymmx2*32*2 + idx_in_ymmx2];
	s[7] = mem[3][n_ymmx2*32*2 + 32 + idx_in_ymmx2];
}

void randomize_1err_ymmx2( uint8_t ** mem )
{
	unsigned n_code = PAGE_SIZE>>1;
	uint8_t ei[2] = {0xff,0xff};
	for(unsigned i=0;i<n_code;i++) {
	//unsigned i=0;
	//{
		unsigned pos = rand()%K;
		xor_ei_ymmx2( ei , pos , i , mem );
	//	if( 0 == i )
	//		printf("\nins 1 err:\npos1: %d, ei: [0x%02x,0x%02x]\n",pos,ei[0],ei[1]);
	}
}


void randomize_2err_ymmx2_ELP( uint8_t ** mem )
{
	unsigned n_code = PAGE_SIZE>>1;
	uint8_t ei[2] = {0xff,0xff};
	uint8_t ej[2] = {0xff,0xff};

	uint8_t ei2[2];
	uint8_t ej2[2];

	uint8_t s[8];
	for(unsigned i=0;i<n_code;i++) {
	//unsigned i=0;
	//{
		uint8_t pos1 = rand()%K;
		uint8_t pos2 = rand()%K;
		while( pos2 == pos1 ) pos2 = rand()%K;
		ei[0] = rand()&0xff;
		ei[1] = rand()&0xff;
		xor_ei_ymmx2( ei , pos1 , i , mem );
		ej[0] = rand()&0xff;
		ej[1] = rand()&0xff;
		xor_ei_ymmx2( ej , pos2 , i , mem );

		s[0] = ei[0]^ej[0];
		s[1] = ei[1]^ej[1];
		s[2] = gf_mul( ei[0] , raid6_q[pos1] ) ^ gf_mul( ej[0] , raid6_q[pos2] );
		s[3] = gf_mul( ei[1] , raid6_q[pos1] ) ^ gf_mul( ej[1] , raid6_q[pos2] );
		s[4] = gf_mul( ei[0] , raid7_sqrt[pos1] ) ^ gf_mul( ej[0] , raid7_sqrt[pos2] );
		s[5] = gf_mul( ei[1] , raid7_sqrt[pos1] ) ^ gf_mul( ej[1] , raid7_sqrt[pos2] );
		*(uint16_t*)&s[6] = gf216_mul( *(uint16_t*)ei , raidq8_X[pos1] ) ^ gf216_mul( *(uint16_t*)ej, raidq8_X[pos2] );
		uint8_t p1,p2;
		if( !decode_2data_err_ELP<K>(ei2,ej2,p1,p2,s) ) {
			xor_ei_ymmx2( ei , pos1 , i , mem );
			xor_ei_ymmx2( ej , pos2 , i , mem );
			i--;
		}
	//	if( 0 == i )
	//		printf("\nins 2 err:\npos1: %d, pos2: %d  ei: [0x%02x,0x%02x], ej:[0x%02x,0x%02x]\n",pos1,pos2,ei[0],ei[1],ej[0],ej[1]);
	}
}


void randomize_2err_ymmx2( uint8_t ** mem )
{
	unsigned n_code = PAGE_SIZE>>1;
	uint8_t ei[2] = {0xff,0xff};
	uint8_t ej[2] = {0xff,0xff};
	for(unsigned i=0;i<n_code;i++) {
	//unsigned i=0;
	//{
		unsigned pos1 = rand()%K;
		unsigned pos2 = rand()%K;
		while( pos2 == pos1 ) pos2 = rand()%K;
		ei[0] = rand()&0xff;
		ei[1] = rand()&0xff;
		xor_ei_ymmx2( ei , pos1 , i , mem );
		ej[0] = rand()&0xff;
		ej[1] = rand()&0xff;
		xor_ei_ymmx2( ej , pos2 , i , mem );
		if( 0 == i )
			printf("\nins 2 err:\npos1: %d, pos2: %d  ei: [0x%02x,0x%02x], ej:[0x%02x,0x%02x]\n",pos1,pos2,ei[0],ei[1],ej[0],ej[1]);
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



int main()
{

	unsigned i;

	uint8_t * mem[N+4];
	uint8_t * mem2[N+4];

	srand(23);

	for(i=0;i<N+4;i++) if( 0 != posix_memalign( (void **) &mem[i] , 32, PAGE_SIZE) ) { printf("mem fail\n"); exit(-1); }
	for(i=0;i<N+4;i++) if( 0 != posix_memalign( (void **) &mem2[i] , 32, PAGE_SIZE) ) { printf("mem fail\n"); exit(-1); }



	randomize_mem( mem , N+4 );


///////////////////////////////////////////


	//typedef ymm_x2 tst_t;

	uint64_t r_ms;
	struct timeval tval_before, tval_after, tval_result;

	mea m1;
	m1.set( K , 4 );

	printf("\n====================\nbenchmark encode\n");

for(int jj = MEASURE_RUN;jj>0;jj--) {
	gettimeofday(&tval_before, NULL);

	for(int qq = TEST_RUN;qq>0;qq--) {
		raidq8_X_gen_syndrome<ymm_x2>(N,PAGE_SIZE,(void**)mem);
	}

	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_result);
	r_ms = ((long int)tval_result.tv_sec)*1000000 + (long int)tval_result.tv_usec;

	m1.ins(r_ms);
}
	m1.say();
	m1.throughput();

	copy_mem( mem2 , mem , N );

	m1.clr();
	unsigned r;

	printf("\n====================\nbenchmark 0 error\n");

for(int jj = MEASURE_RUN;jj>0;jj--) {
	gettimeofday(&tval_before, NULL);

	for(int qq = TEST_RUN;qq>0;qq--) {
		raidq8_X_diff_syndrome<ymm_x2>( K, PAGE_SIZE, (void **) mem2 );
		r = raidq8_X_error_decoder_ymmx2<K>( PAGE_SIZE , (void **) &mem2[N] );
	}

	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_result);
	r_ms = ((long int)tval_result.tv_sec)*1000000 + (long int)tval_result.tv_usec;

	m1.ins(r_ms);
}
	m1.say();
	m1.throughput();



	printf("r = raidq8_X_error_decoder_ymmx2(): %d\n", r );

//////////////////////////////////////////////////
	uint8_t s[8];
	uint8_t ei[2];
	uint8_t ej[2];
	unsigned succ = 0;

	/// 1 data err

	printf("\n====================\nbenchmark 1 error\n");

	copy_mem( mem2 , mem , N );
	randomize_1err_ymmx2( mem2 );

	m1.clr();
for(int jj = MEASURE_RUN;jj>0;jj--) {
	gettimeofday(&tval_before, NULL);

	for(int qq = TEST_RUN;qq>0;qq--) {

		raidq8_X_diff_syndrome<ymm_x2>( K, PAGE_SIZE, (void **) mem2 );

		for(i=0;i<PAGE_SIZE/2;i++) {
			get_synds_ymmx2( s , i , &mem2[N] );
			uint8_t pos;
			if( decode_1data_err<K>( pos , s ) ) succ ++;
		}

	}

	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_result);
	r_ms = ((long int)tval_result.tv_sec)*1000000 + (long int)tval_result.tv_usec;

	m1.ins(r_ms);
}
	m1.say();
	m1.throughput();


	printf( "%d : 1 err corrected.\n", succ );

	/// 2 data err 

	copy_mem( mem2 , mem , N );
	randomize_2err_ymmx2_ELP( mem2 );

	/// ELP


	printf("\n====================\nbenchmark 2 errors ELP\n");

	succ = 0;
	m1.clr();
for(int jj = MEASURE_RUN/10;jj>0;jj--) {
	gettimeofday(&tval_before, NULL);

	for(int qq = TEST_RUN;qq>0;qq--) {

		raidq8_X_diff_syndrome<ymm_x2>( K, PAGE_SIZE, (void **) mem2 );

		for(i=0;i<PAGE_SIZE/2;i++) {
			get_synds_ymmx2( s , i , &mem2[N] );
			uint8_t pi=0,pj=0;
			if( decode_2data_err_ELP<K>( ei , ej , pi , pj , s ) ) succ ++;
		}
	}

	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_result);
	r_ms = ((long int)tval_result.tv_sec)*1000000 + (long int)tval_result.tv_usec;

	m1.ins(r_ms);
}
	m1.say();
	m1.throughput();


	printf( "%d : 2 err corrected (ELP).\n", succ );

	/// enum

	printf("\n====================\nbenchmark 2 errors enum\n");

	succ = 0;
	m1.clr();
for(int jj = 3;jj>0;jj--) {
	gettimeofday(&tval_before, NULL);

	for(int qq = TEST_RUN;qq>0;qq--) {

		raidq8_X_diff_syndrome<ymm_x2>( K, PAGE_SIZE, (void **) mem2 );

		for(i=0;i<PAGE_SIZE/2;i++) {
			get_synds_ymmx2( s , i , &mem2[N] );
			uint8_t pi=0,pj=0;
			if( decode_2data_err_enum<K>( ei , ej , pi , pj , s ) ) succ ++;
		}
	}

	gettimeofday(&tval_after, NULL);
	timersub(&tval_after, &tval_before, &tval_result);
	r_ms = ((long int)tval_result.tv_sec)*1000000 + (long int)tval_result.tv_usec;

	m1.ins(r_ms);
}
	m1.say();
	m1.throughput();


	printf( "%d : 2 err corrected (enum).\n", succ );

	/// checked code

	///printf("raid8 X passed.\n");


	return 0;
}

