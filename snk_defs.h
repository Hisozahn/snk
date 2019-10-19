#ifndef __SNK_DEFS_H__
#define __SNK_DEFS_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Direction of an object */
typedef enum snk_direction {
    SNK_DIRECTION_LEFT,
    SNK_DIRECTION_UP,
    SNK_DIRECTION_RIGHT,
    SNK_DIRECTION_DOWN,
    SNK_DIRECTION_INVALID,
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
        case SNK_DIRECTION_RIGHT:
            return SNK_DIRECTION_LEFT;
        case SNK_DIRECTION_LEFT:
            return SNK_DIRECTION_RIGHT;
        case SNK_DIRECTION_DOWN:
            return SNK_DIRECTION_UP;
        case SNK_DIRECTION_UP:
            return SNK_DIRECTION_DOWN;
        default:
            return SNK_DIRECTION_INVALID;
    }
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_DEFS_H__ */
