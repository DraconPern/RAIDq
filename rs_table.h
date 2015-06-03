#ifndef _RS_TABLE_H_
#define _RS_TABLE_H_

typedef unsigned char uint8_t;

#define RS_MAX_MSG (128)
#define RS_MAX_CKS (8)

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t rs_cks0[];
extern const uint8_t rs_cks1[];
extern const uint8_t rs_cks2[];
extern const uint8_t rs_cks3[];
extern const uint8_t rs_cks4[];
extern const uint8_t rs_cks5[];
extern const uint8_t rs_cks6[];
extern const uint8_t rs_cks7[];

extern const uint8_t * raid_rs[];

#ifdef __cplusplus
}
#endif


#endif

