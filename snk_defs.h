#ifndef __SNK_DEFS_H__
#define __SNK_DEFS_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Status code.
 */
typedef enum snk_rc_type {
    SNK_RC_SUCCESS, /**< OK */
    SNK_RC_OVER, /**< Game over condition is reached */
    SNK_RC_INVALID, /**< Invalid arguments/object state */
    SNK_RC_NOBUF, /**< Buffer size is too small */
    SNK_RC_NOENT, /**< No such entry */
} snk_rc_type;

/** Direction of an object */
typedef enum snk_direction {
    SNK_DIRECTION_LEFT, /**< Towards negative x coordinate */
    SNK_DIRECTION_UP, /**< Towards negative y coordinate */
    SNK_DIRECTION_RIGHT, /**< Towards positive x coordinate */
    SNK_DIRECTION_DOWN, /**< Towards positive y coordinate */
    SNK_DIRECTION_INVALID, /**< Invalid direction */
    SNK_DIRECTION_ENUM_VALID_SIZE = SNK_DIRECTION_INVALID, /**< Number of valid enum entries */
} snk_direction;

/**
 * Get opposite direction.
 *
 * @param[in] direction     Original direction
 *
 * @return                  Opposite direction
 */
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

static inline int
snk_direction_is_valid(snk_direction direction)
{
    return (direction >= 0 && direction < SNK_DIRECTION_ENUM_VALID_SIZE);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_DEFS_H__ */
