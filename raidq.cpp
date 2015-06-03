#include <stddef.h>

#include "raidq.h"
#include "raidq.hpp"


extern "C"
void raid5_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raid5_gen_syndrome<uint64_t>(disks,bytes,ptrs);
}

extern "C"
void raid5_xmmx1_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raid5_gen_syndrome<__m128i>(disks,bytes,ptrs);
}

extern "C"
void raid5_xmmx2_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raid5_gen_syndrome<xmm_x2>(disks,bytes,ptrs);
}

extern "C"
void raid5_xmmx4_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raid5_gen_syndrome<xmm_x4>(disks,bytes,ptrs);
}



//////////////////////////

extern "C"
void raid5_uint64_recov(int disks, size_t bytes, int faila, void **ptrs)
{
	void * tmp = ptrs[faila];
	ptrs[faila]=ptrs[disks-1];
	ptrs[disks-1]=tmp;
	raid5_uint64_gen_syndrome(disks,bytes,ptrs);
	tmp = ptrs[faila];
	ptrs[faila]=ptrs[disks-1];
	ptrs[disks-1]=tmp;
}

extern "C"
void raid5_xmmx1_recov(int disks, size_t bytes, int faila, void **ptrs)
{
	void * tmp = ptrs[faila];
	ptrs[faila]=ptrs[disks-1];
	ptrs[disks-1]=tmp;
	raid5_xmmx1_gen_syndrome(disks,bytes,ptrs);
	tmp = ptrs[faila];
	ptrs[faila]=ptrs[disks-1];
	ptrs[disks-1]=tmp;
}

extern "C"
void raid5_xmmx2_recov(int disks, size_t bytes, int faila, void **ptrs)
{
	void * tmp = ptrs[faila];
	ptrs[faila]=ptrs[disks-1];
	ptrs[disks-1]=tmp;
	raid5_xmmx2_gen_syndrome(disks,bytes,ptrs);
	tmp = ptrs[faila];
	ptrs[faila]=ptrs[disks-1];
	ptrs[disks-1]=tmp;
}

extern "C"
void raid5_xmmx4_recov(int disks, size_t bytes, int faila, void **ptrs)
{
	void * tmp = ptrs[faila];
	ptrs[faila]=ptrs[disks-1];
	ptrs[disks-1]=tmp;
	raid5_xmmx4_gen_syndrome(disks,bytes,ptrs);
	tmp = ptrs[faila];
	ptrs[faila]=ptrs[disks-1];
	ptrs[disks-1]=tmp;
}





////////////////////////////////////////////



extern "C"
void raid6_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raid6_gen_syndrome<uint64_t>(disks,bytes,ptrs);
}

extern "C"
void raid6_xmmx1_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raid6_gen_syndrome<__m128i>(disks,bytes,ptrs);
}

extern "C"
void raid6_xmmx2_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raid6_gen_syndrome<xmm_x2>(disks,bytes,ptrs);
}

extern "C"
void raid6_xmmx4_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raid6_gen_syndrome<xmm_x4>(disks,bytes,ptrs);
}



//////////////////////////////////////










//////////////////////////////////////////////////



extern "C"
void raidq7_a2_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raidq7_a2_gen_syndrome<uint64_t>(disks,bytes,ptrs);
}

extern "C"
void raidq7_a2_xmmx1_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raidq7_a2_gen_syndrome<__m128i>(disks,bytes,ptrs);
}

extern "C"
void raidq7_a2_xmmx2_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raidq7_a2_gen_syndrome<xmm_x2>(disks,bytes,ptrs);
}

extern "C"
void raidq7_a2_xmmx4_gen_syndrome(int disks, size_t bytes, void **ptrs)
{
	raidq7_a2_gen_syndrome<xmm_x4>(disks,bytes,ptrs);
}





/////////////////////////////////////////

