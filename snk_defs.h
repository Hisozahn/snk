#ifndef __SNK_DEFS_H__
#define __SNK_DEFS_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum snk_rc_type {
    SNK_RC_SUCCESS,
    SNK_RC_OVER,
    SNK_RC_INVALID,
    SNK_RC_NOBUF,
    SNK_RC_NOENT,
} snk_rc_type;

/** Direction of an object */
typedef enum snk_direction {
    SNK_DIRECTION_LEFT,
    SNK_DIRECTION_UP,
    SNK_DIRECTION_RIGHT,
    SNK_DIRECTION_DOWN,
    SNK_DIRECTION_INVALID,
} snk_direction;

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
