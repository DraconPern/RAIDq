
#ifndef _RECOVER_HPP_
#define _RECOVER_HPP_

#include "recover.h"

#include <assert.h>


template <unsigned P >
static void get_rec_coef( uint8_t *rec_coe[] , uint8_t idx[], const uint8_t * coef[] )
{
	assert(0);
}


template <>
void get_rec_coef<2>( uint8_t *rec_coe[], uint8_t idx[], const uint8_t * coef[] )
{
	get_rec_coef2( rec_coe , idx[0], idx[1], coef );
}

template <>
void get_rec_coef<3>( uint8_t *rec_coe[], uint8_t idx[], const uint8_t * coef[] )
{
	get_rec_coef3( rec_coe , idx[0] , idx[1], idx[2], coef );
}

template <>
void get_rec_coef<4>( uint8_t *rec_coe[], uint8_t idx[], const uint8_t * coef[] )
{
	get_rec_coef4( rec_coe , idx , coef );
}



#endif /// _RECOVER_HPP_


