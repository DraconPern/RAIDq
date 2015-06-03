#ifndef _RAID6_RS_H_
#define _RAID6_RS_H_


#include <stddef.h>

#ifndef _D_U8_
#define _D_U8_
typedef unsigned char uint8_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

void raid6_rs_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid6_rs_ssse3x1_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid6_rs_ssse3x2_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid6_rs_ssse3x4_gen_syndrome(int disks, size_t bytes, void **ptrs);


void raid6_rs_uint64_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs);
void raid6_rs_ssse3x1_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs);
void raid6_rs_ssse3x2_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs);
void raid6_rs_ssse3x4_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs);





void raid7_rs_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid7_rs_ssse3x1_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid7_rs_ssse3x2_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid7_rs_ssse3x4_gen_syndrome(int disks, size_t bytes, void **ptrs);


void raid7_rs_uint64_3fails_recov(int disks, size_t bytes, int faila, int failb, int failc, void **ptrs);
void raid7_rs_ssse3x1_3fails_recov(int disks, size_t bytes, int faila, int failb, int failc, void **ptrs);
void raid7_rs_ssse3x2_3fails_recov(int disks, size_t bytes, int faila, int failb, int failc, void **ptrs);
void raid7_rs_ssse3x4_3fails_recov(int disks, size_t bytes, int faila, int failb, int failc, void **ptrs);





void raid8_rs_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid8_rs_ssse3x1_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid8_rs_ssse3x2_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid8_rs_ssse3x4_gen_syndrome(int disks, size_t bytes, void **ptrs);


void raid8_rs_uint64_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raid8_rs_ssse3x1_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raid8_rs_ssse3x2_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raid8_rs_ssse3x4_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);




#ifdef __cplusplus
}
#endif




#endif
