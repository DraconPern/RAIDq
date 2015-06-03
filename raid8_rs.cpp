
#include "raid8_rs.h"
#include "raid_rs.hpp"
#include "rs_table.h"

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

//////////////////////

extern "C"
void raid8_rs_ssse3x1_1fail_recov(int disks, size_t bytes, int faila, void **ptrs)
{
	typedef __m128i sto_t;
	uint8_t fail_idx[1];
	fail_idx[0] = faila;
	if(faila<disks-4) raid_rs_recov<sto_t,1,0,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,1,4>(disks-4,bytes,fail_idx,ptrs);
}


extern "C"
void raid8_rs_ssse3x2_1fail_recov(int disks, size_t bytes, int faila, void **ptrs)
{
	typedef xmm_x2 sto_t;
	uint8_t fail_idx[1];
	fail_idx[0] = faila;
	if(faila<disks-4) raid_rs_recov<sto_t,1,0,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,1,4>(disks-4,bytes,fail_idx,ptrs);
}


extern "C"
void raid8_rs_ssse3x4_1fail_recov(int disks, size_t bytes, int faila, void **ptrs)
{
	typedef xmm_x4 sto_t;
	uint8_t fail_idx[1];
	fail_idx[0] = faila;
	if(faila<disks-4) raid_rs_recov<sto_t,1,0,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,1,4>(disks-4,bytes,fail_idx,ptrs);
}

/////////////////////////////

extern "C"
void raid8_rs_ssse3x1_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs)
{
	typedef __m128i sto_t;

	if(faila>failb) { int temp=failb; failb=faila; faila=temp; }
	uint8_t fail_idx[2]; fail_idx[0]=faila; fail_idx[1]=failb;
	if(failb<disks-4) raid_rs_recov<sto_t,2,0,4>(disks-4,bytes,fail_idx,ptrs);
	else if(faila<disks-4) raid_rs_recov<sto_t,1,1,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,2,4>(disks-4,bytes,fail_idx,ptrs);
}

extern "C"
void raid8_rs_ssse3x2_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs)
{
	typedef xmm_x2 sto_t;

	if(faila>failb) { int temp=failb; failb=faila; faila=temp; }
	uint8_t fail_idx[2]; fail_idx[0]=faila; fail_idx[1]=failb;
	if(failb<disks-4) raid_rs_recov<sto_t,2,0,4>(disks-4,bytes,fail_idx,ptrs);
	else if(faila<disks-4) raid_rs_recov<sto_t,1,1,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,2,4>(disks-4,bytes,fail_idx,ptrs);
}

extern "C"
void raid8_rs_ssse3x4_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs)
{
	typedef xmm_x4 sto_t;

	if(faila>failb) { int temp=failb; failb=faila; faila=temp; }
	uint8_t fail_idx[2]; fail_idx[0]=faila; fail_idx[1]=failb;
	if(failb<disks-4) raid_rs_recov<sto_t,2,0,4>(disks-4,bytes,fail_idx,ptrs);
	else if(faila<disks-4) raid_rs_recov<sto_t,1,1,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,2,4>(disks-4,bytes,fail_idx,ptrs);
}


//////////////////////////////////////////////////




extern "C"
void raid8_rs_ssse3x1_3fails_recov(int disks, size_t bytes, int faila, int failb, int failc, void **ptrs)
{
	typedef __m128i sto_t;

	if(faila>failb) {int temp=failb; failb=faila; faila=temp;}
	if(faila>failc) {int temp=failc; failc=faila; faila=temp;}
	if(failb>failc) {int temp=failb; failb=failc; failc=temp;}
	uint8_t fail_idx[3]; fail_idx[0]=faila; fail_idx[1]=failb; fail_idx[2]=failc;

	if(failc<disks-4) raid_rs_recov<sto_t,3,0,4>(disks-4,bytes,fail_idx,ptrs);
	else if(failb<disks-4) raid_rs_recov<sto_t,2,1,4>(disks-4,bytes,fail_idx,ptrs);
	else if(faila<disks-4) raid_rs_recov<sto_t,1,2,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,3,4>(disks-4,bytes,fail_idx,ptrs);
}

extern "C"
void raid8_rs_ssse3x2_3fails_recov(int disks, size_t bytes, int faila, int failb, int failc, void **ptrs)
{
	typedef xmm_x2 sto_t;

	if(faila>failb) {int temp=failb; failb=faila; faila=temp;}
	if(faila>failc) {int temp=failc; failc=faila; faila=temp;}
	if(failb>failc) {int temp=failb; failb=failc; failc=temp;}
	uint8_t fail_idx[3]; fail_idx[0]=faila; fail_idx[1]=failb; fail_idx[2]=failc;

	if(failc<disks-4) raid_rs_recov<sto_t,3,0,4>(disks-4,bytes,fail_idx,ptrs);
	else if(failb<disks-4) raid_rs_recov<sto_t,2,1,4>(disks-4,bytes,fail_idx,ptrs);
	else if(faila<disks-4) raid_rs_recov<sto_t,1,2,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,3,4>(disks-4,bytes,fail_idx,ptrs);
}

extern "C"
void raid8_rs_ssse3x4_3fails_recov(int disks, size_t bytes, int faila, int failb, int failc, void **ptrs)
{
	typedef xmm_x4 sto_t;

	if(faila>failb) {int temp=failb; failb=faila; faila=temp;}
	if(faila>failc) {int temp=failc; failc=faila; faila=temp;}
	if(failb>failc) {int temp=failb; failb=failc; failc=temp;}
	uint8_t fail_idx[3]; fail_idx[0]=faila; fail_idx[1]=failb; fail_idx[2]=failc;

	if(failc<disks-4) raid_rs_recov<sto_t,3,0,4>(disks-4,bytes,fail_idx,ptrs);
	else if(failb<disks-4) raid_rs_recov<sto_t,2,1,4>(disks-4,bytes,fail_idx,ptrs);
	else if(faila<disks-4) raid_rs_recov<sto_t,1,2,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,3,4>(disks-4,bytes,fail_idx,ptrs);
}


////////////////////////////////////////////


extern "C"
void raid8_rs_ssse3x1_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs)
{
	typedef __m128i sto_t;

   uint8_t tmp;
	if(fail_idx[0]>fail_idx[1]) { tmp=fail_idx[0]; fail_idx[0]=fail_idx[1]; fail_idx[1]=tmp; }
	if(fail_idx[0]>fail_idx[2]) { tmp=fail_idx[0]; fail_idx[0]=fail_idx[2]; fail_idx[2]=tmp; }
	if(fail_idx[0]>fail_idx[3]) { tmp=fail_idx[0]; fail_idx[0]=fail_idx[3]; fail_idx[3]=tmp; }
	if(fail_idx[1]>fail_idx[2]) { tmp=fail_idx[1]; fail_idx[1]=fail_idx[2]; fail_idx[2]=tmp; }
	if(fail_idx[1]>fail_idx[3]) { tmp=fail_idx[1]; fail_idx[1]=fail_idx[3]; fail_idx[3]=tmp; }
	if(fail_idx[2]>fail_idx[3]) { tmp=fail_idx[2]; fail_idx[2]=fail_idx[3]; fail_idx[3]=tmp; }

	if(fail_idx[3]<disks-4) raid_rs_recov<sto_t,4,0,4>(disks-4,bytes,fail_idx,ptrs);
	else if(fail_idx[2]<disks-4) raid_rs_recov<sto_t,3,1,4>(disks-4,bytes,fail_idx,ptrs);
	else if(fail_idx[1]<disks-4) raid_rs_recov<sto_t,2,2,4>(disks-4,bytes,fail_idx,ptrs);
	else if(fail_idx[0]<disks-4) raid_rs_recov<sto_t,1,3,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,4,4>(disks-4,bytes,fail_idx,ptrs);
}




extern "C"
void raid8_rs_ssse3x2_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs)
{
	typedef xmm_x2 sto_t;

   uint8_t tmp;
	if(fail_idx[0]>fail_idx[1]) { tmp=fail_idx[0]; fail_idx[0]=fail_idx[1]; fail_idx[1]=tmp; }
	if(fail_idx[0]>fail_idx[2]) { tmp=fail_idx[0]; fail_idx[0]=fail_idx[2]; fail_idx[2]=tmp; }
	if(fail_idx[0]>fail_idx[3]) { tmp=fail_idx[0]; fail_idx[0]=fail_idx[3]; fail_idx[3]=tmp; }
	if(fail_idx[1]>fail_idx[2]) { tmp=fail_idx[1]; fail_idx[1]=fail_idx[2]; fail_idx[2]=tmp; }
	if(fail_idx[1]>fail_idx[3]) { tmp=fail_idx[1]; fail_idx[1]=fail_idx[3]; fail_idx[3]=tmp; }
	if(fail_idx[2]>fail_idx[3]) { tmp=fail_idx[2]; fail_idx[2]=fail_idx[3]; fail_idx[3]=tmp; }

	if(fail_idx[3]<disks-4) raid_rs_recov<sto_t,4,0,4>(disks-4,bytes,fail_idx,ptrs);
	else if(fail_idx[2]<disks-4) raid_rs_recov<sto_t,3,1,4>(disks-4,bytes,fail_idx,ptrs);
	else if(fail_idx[1]<disks-4) raid_rs_recov<sto_t,2,2,4>(disks-4,bytes,fail_idx,ptrs);
	else if(fail_idx[0]<disks-4) raid_rs_recov<sto_t,1,3,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,4,4>(disks-4,bytes,fail_idx,ptrs);
}




extern "C"
void raid8_rs_ssse3x4_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs)
{
	typedef xmm_x4 sto_t;

   uint8_t tmp;
	if(fail_idx[0]>fail_idx[1]) { tmp=fail_idx[0]; fail_idx[0]=fail_idx[1]; fail_idx[1]=tmp; }
	if(fail_idx[0]>fail_idx[2]) { tmp=fail_idx[0]; fail_idx[0]=fail_idx[2]; fail_idx[2]=tmp; }
	if(fail_idx[0]>fail_idx[3]) { tmp=fail_idx[0]; fail_idx[0]=fail_idx[3]; fail_idx[3]=tmp; }
	if(fail_idx[1]>fail_idx[2]) { tmp=fail_idx[1]; fail_idx[1]=fail_idx[2]; fail_idx[2]=tmp; }
	if(fail_idx[1]>fail_idx[3]) { tmp=fail_idx[1]; fail_idx[1]=fail_idx[3]; fail_idx[3]=tmp; }
	if(fail_idx[2]>fail_idx[3]) { tmp=fail_idx[2]; fail_idx[2]=fail_idx[3]; fail_idx[3]=tmp; }

	if(fail_idx[3]<disks-4) raid_rs_recov<sto_t,4,0,4>(disks-4,bytes,fail_idx,ptrs);
	else if(fail_idx[2]<disks-4) raid_rs_recov<sto_t,3,1,4>(disks-4,bytes,fail_idx,ptrs);
	else if(fail_idx[1]<disks-4) raid_rs_recov<sto_t,2,2,4>(disks-4,bytes,fail_idx,ptrs);
	else if(fail_idx[0]<disks-4) raid_rs_recov<sto_t,1,3,4>(disks-4,bytes,fail_idx,ptrs);
	else raid_rs_recov<sto_t,0,4,4>(disks-4,bytes,fail_idx,ptrs);
}


