#include "stdlib.h"
#include "string.h"
#include <stdio.h>
#include <assert.h>

#include "stdint.h"

#include "raid_table.h"
#include "raidq.h"

#include "raidq.hpp"

#include "error_decoder_q.hpp"


#define K 32
#define N (K+4)


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
		if( 0 == i )
			printf("\nins 1 err:\npos1: %d, ei: [0x%02x,0x%02x]\n",pos,ei[0],ei[1]);
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

	raidq8_X_gen_syndrome<ymm_x2>(N,PAGE_SIZE,(void**)mem);

	copy_mem( mem2 , mem , N );

	raidq8_X_diff_syndrome<ymm_x2>( K, PAGE_SIZE, (void **) mem2 );
	unsigned r = raidq8_X_error_decoder_ymmx2<K>( PAGE_SIZE , (void **) &mem2[N] );

	printf("r = raidq8_X_error_decoder_ymmx2(): %d\n", r );

//////////////////////////////////////////////////
	uint8_t s[8];
	uint8_t ei[2];
	uint8_t ej[2];

	/// 1 data err

	copy_mem( mem2 , mem , N );
	randomize_1err_ymmx2( mem2 );
	raidq8_X_diff_syndrome<ymm_x2>( K, PAGE_SIZE, (void **) mem2 );

	unsigned succ = 0;
	for(i=0;i<PAGE_SIZE/2;i++) {
		get_synds_ymmx2( s , i , &mem2[N] );
		uint8_t pos;
		if( decode_1data_err<K>( pos , s ) ) succ ++;
		if( 0 == i ) {
			printf("===========================\n");
			printf("s[]: ");
			for(unsigned j=0;j<8;j++) printf("0x%02x, ", s[j]);
			printf("\npos: %d\n" , pos );
		}
	}
	printf( "%d : 1 err corrected.\n", succ );

	/// 2 data err 

	copy_mem( mem2 , mem , N );
	randomize_2err_ymmx2( mem2 );
	raidq8_X_diff_syndrome<ymm_x2>( K, PAGE_SIZE, (void **) mem2 );

	/// ELP

	succ = 0;
	for(i=0;i<PAGE_SIZE/2;i++) {
		get_synds_ymmx2( s , i , &mem2[N] );
		uint8_t pi=0,pj=0;
		if( decode_2data_err_ELP<K>( ei , ej , pi , pj , s ) ) succ ++;
		if( 0 == i ) {
			printf("===========================\n");
			printf("s[]: ");
			for(unsigned j=0;j<8;j++) printf("0x%02x, ", s[j]);
			printf("\n i, j, ei[2], ej[2]: %d, %d, [0x%02x,0x%02x], [0x%02x,0x%02x]\n" , pi, pj , ei[0],ei[1], ej[0], ej[1] );

		}
	}
	printf( "%d : 2 err corrected (ELP).\n", succ );

	/// enum

	succ = 0;
	for(i=0;i<PAGE_SIZE/2;i++) {
		get_synds_ymmx2( s , i , &mem2[N] );
		uint8_t pi=0,pj=0;
		if( decode_2data_err_enum<K>( ei , ej , pi , pj , s ) ) succ ++;
		if( 0 == i ) {
			printf("===========================\n");
			printf("s[]: ");
			for(unsigned j=0;j<8;j++) printf("0x%02x, ", s[j]);
			printf("\n i, j, ei[2], ej[2]: %d, %d, [0x%02x,0x%02x], [0x%02x,0x%02x]\n" , pi, pj , ei[0],ei[1], ej[0], ej[1] );

		}
	}
	printf( "%d : 2 err corrected (enum).\n", succ );

	/// checked code

	printf("raid8 X passed.\n");


	return 0;
}

