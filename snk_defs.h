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
    SNK_INVALID,
} snk_direction;

/** Position on a field */
typedef struct snk_position {
    /** Horizontal coordinate, starting from left border */
    uint32_t x;
    /** Vertical coordinate, starting from upper border */
    uint32_t y;
} snk_position;

static inline snk_direction
snk_direction_reverse(snk_direction direction)
{
    switch (direction)
    {
        case SNK_RIGHT:
            return SNK_LEFT;
        case SNK_LEFT:
            return SNK_RIGHT;
        case SNK_DOWN:
            return SNK_UP;
        case SNK_UP:
            return SNK_DOWN;
        default:
            return SNK_INVALID;
    }
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_DEFS_H__ */
