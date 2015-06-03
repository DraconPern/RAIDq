#ifndef _RAIDQ_H_
#define _RAIDQ_H_


#include <stddef.h>

#ifndef _D_U8_
#define _D_U8_
typedef unsigned char uint8_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

void raid5_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid5_xmmx1_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid5_xmmx2_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid5_xmmx4_gen_syndrome(int disks, size_t bytes, void **ptrs);

void raid5_uint64_recov(int disks, size_t bytes, int faila, void **ptrs);
void raid5_xmmx1_recov(int disks, size_t bytes, int faila, void **ptrs);
void raid5_xmmx2_recov(int disks, size_t bytes, int faila, void **ptrs);
void raid5_xmmx4_recov(int disks, size_t bytes, int faila, void **ptrs);





void raid6_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid6_xmmx1_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid6_xmmx2_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raid6_xmmx4_gen_syndrome(int disks, size_t bytes, void **ptrs);

void raid6_uint64_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs);
void raid6_xmmx1_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs);
void raid6_xmmx2_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs);
void raid6_xmmx4_2fails_recov(int disks, size_t bytes, int faila, int failb, void **ptrs);





void raidq7_a2_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raidq7_a2_xmmx1_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raidq7_a2_xmmx2_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raidq7_a2_xmmx4_gen_syndrome(int disks, size_t bytes, void **ptrs);

void raidq7_a2_uint64_3fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raidq7_a2_xmmx1_3fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raidq7_a2_xmmx2_3fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raidq7_a2_xmmx4_3fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);




void raidq7_sqrta_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raidq7_sqrta_xmmx1_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raidq7_sqrta_xmmx2_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raidq7_sqrta_xmmx4_gen_syndrome(int disks, size_t bytes, void **ptrs);

void raidq7_sqrta_uint64_3fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raidq7_sqrta_xmmx1_3fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raidq7_sqrta_xmmx2_3fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raidq7_sqrta_xmmx4_3fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);





void raidq8_X_uint64_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raidq8_X_ssse3x1_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raidq8_X_ssse3x2_gen_syndrome(int disks, size_t bytes, void **ptrs);
void raidq8_X_ssse3x4_gen_syndrome(int disks, size_t bytes, void **ptrs);

void raidq8_X_uint64_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raidq8_X_ssse3x1_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raidq8_X_ssse3x2_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);
void raidq8_X_ssse3x4_4fails_recov(int disks, size_t bytes, uint8_t fail_idx[], void **ptrs);





#ifdef __cplusplus
}
#endif




#endif
