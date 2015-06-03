#ifndef _RAID_TABLE_H_
#define _RAID_TABLE_H_


#define PAGE_SIZE 4096

#define RAID_PLANK_MAX_DISKS (128)

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#ifdef __cplusplus
extern "C" {
#endif

extern const char raid6_empty_zero_page[PAGE_SIZE];

extern const uint8_t raid6_q[];

extern const uint8_t inv_a_to_n_p_1[];

extern const uint8_t raid5_xor[];

extern const uint8_t raid7_alpha2[];

extern const uint8_t raid7_sqrt[];

extern const uint8_t * raid_sqrt[];

extern const uint8_t * raid_a2[];

extern const uint16_t raidq8_X[];

extern const uint16_t raidq8_a2Xp1[];

extern const uint16_t raidq8_a141X[];

extern const uint16_t raidq8_a186Xpa6[];

#ifdef __cplusplus
}
#endif


#endif

