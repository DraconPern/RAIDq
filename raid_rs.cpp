#include <stddef.h>

#include "rs_table.h"
#include "raid_rs.h"
#include "raid_rs.hpp"


extern "C"
void raid6_rs_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<uint64_t,2>(disks,bytes,ptrs,raid_rs);
}


extern "C"
void raid6_rs_ssse3x1_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<__m128i,2>(disks,bytes,ptrs,raid_rs);
}

extern "C"
void raid6_rs_ssse3x2_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<xmm_x2,2>(disks,bytes,ptrs,raid_rs);
}

extern "C"
void raid6_rs_ssse3x4_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<xmm_x4,2>(disks,bytes,ptrs,raid_rs);
}

//////////////////////



extern "C"
void raid7_rs_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<uint64_t,3>(disks,bytes,ptrs,raid_rs);
}

extern "C"
void raid7_rs_ssse3x1_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<__m128i,3>(disks,bytes,ptrs,raid_rs);
}

extern "C"
void raid7_rs_ssse3x2_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<xmm_x2,3>(disks,bytes,ptrs,raid_rs);
}

extern "C"
void raid7_rs_ssse3x4_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<xmm_x4,3>(disks,bytes,ptrs,raid_rs);
}



//////////////////////


extern "C"
void raid8_rs_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<uint64_t,4>(disks,bytes,ptrs,raid_rs);
}

extern "C"
void raid8_rs_ssse3x1_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<__m128i,4>(disks,bytes,ptrs,raid_rs);
}

extern "C"
void raid8_rs_ssse3x2_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<xmm_x2,4>(disks,bytes,ptrs,raid_rs);
}

extern "C"
void raid8_rs_ssse3x4_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	gen_syndrome<xmm_x4,4>(disks,bytes,ptrs,raid_rs);
}



//////////////////////////



extern "C"
void raid6_rs_uint64_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs)
{
	uint8_t fail_idx[2];
	fail_idx[0] = faila;
	fail_idx[1] = failb;
	raid_rs_recov_data<uint64_t,2>(disks-2,bytes,fail_idx,ptrs,raid_rs);
}


extern "C"
void raid6_rs_ssse3x1_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs)
{
	uint8_t fail_idx[2];
	fail_idx[0] = faila;
	fail_idx[1] = failb;
	raid_rs_recov_data<__m128i,2>(disks-2,bytes,fail_idx,ptrs,raid_rs);
}

extern "C"
void raid6_rs_ssse3x2_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs)
{
	uint8_t fail_idx[2];
	fail_idx[0] = faila;
	fail_idx[1] = failb;
	raid_rs_recov_data<xmm_x2,2>(disks-2,bytes,fail_idx,ptrs,raid_rs);
}

extern "C"
void raid6_rs_ssse3x4_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs)
{
	uint8_t fail_idx[2];
	fail_idx[0] = faila;
	fail_idx[1] = failb;
	raid_rs_recov_data<xmm_x4,2>(disks-2,bytes,fail_idx,ptrs,raid_rs);
}




//////////////////////////////////////////////////




extern "C"
void raid7_rs_uint64_3fails_recov(int disks, size_t bytes, int faila, int failb, int failc, void **ptrs)
{
	uint8_t fail_idx[3];
	fail_idx[0] = faila;
	fail_idx[1] = failb;
	fail_idx[2] = failc;
	raid_rs_recov_data<uint64_t,3>(disks-3,bytes,fail_idx,ptrs,raid_rs);
}

extern "C"
void raid7_rs_ssse3x1_3fails_recov(int disks, size_t bytes, int faila, int failb, int failc, void **ptrs)
{
	uint8_t fail_idx[3];
	fail_idx[0] = faila;
	fail_idx[1] = failb;
	fail_idx[2] = failc;
	raid_rs_recov_data<__m128i,3>(disks-3,bytes,fail_idx,ptrs,raid_rs);
}

extern "C"
void raid7_rs_ssse3x2_3fails_recov(int disks, size_t bytes, int faila, int failb, int failc, void **ptrs)
{
	uint8_t fail_idx[3];
	fail_idx[0] = faila;
	fail_idx[1] = failb;
	fail_idx[2] = failc;
	raid_rs_recov_data<xmm_x2,3>(disks-3,bytes,fail_idx,ptrs,raid_rs);
}

extern "C"
void raid7_rs_ssse3x4_3fails_recov(int disks, size_t bytes, int faila, int failb, int failc, void **ptrs)
{
	uint8_t fail_idx[3];
	fail_idx[0] = faila;
	fail_idx[1] = failb;
	fail_idx[2] = failc;
	raid_rs_recov_data<xmm_x4,3>(disks-3,bytes,fail_idx,ptrs,raid_rs);
}




////////////////////////////////////////////




extern "C"
void raid8_rs_uint64_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs)
{
	raid_rs_recov_data<uint64_t,4>(disks-4,bytes,fail_idx,ptrs,raid_rs);
}

extern "C"
void raid8_rs_ssse3x1_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs)
{
	raid_rs_recov_data<__m128i,4>(disks-4,bytes,fail_idx,ptrs,raid_rs);
}

extern "C"
void raid8_rs_ssse3x2_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs)
{
	raid_rs_recov_data<xmm_x2,4>(disks-4,bytes,fail_idx,ptrs,raid_rs);
}

extern "C"
void raid8_rs_ssse3x4_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs)
{
	raid_rs_recov_data<xmm_x4,4>(disks-4,bytes,fail_idx,ptrs,raid_rs);
}





