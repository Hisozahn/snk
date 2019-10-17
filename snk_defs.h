#ifndef __SNK_DEFS_H__
#define __SNK_DEFS_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Direction of an object */
typedef enum snk_direction {
    SNK_LEFT,
    SNK_UP,
    SNK_RIGHT,
    SNK_DOWN,
} snk_direction;

/** Position on a field */
typedef struct snk_position {
    /** Horizontal coordinate, starting from left border */
    uint8_t x;
    /** Vertical coordinate, starting from upper border */
    uint8_t y;
} snk_position;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_DEFS_H__ */
