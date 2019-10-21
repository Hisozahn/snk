#ifndef __SNK_POSITION_H__
#define __SNK_POSITION_H__

#include "snk_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Position on a finite rectangular surface */
typedef struct snk_position {
    /** Horizontal coordinate, starting from left border */
    uint32_t x;
    /** Vertical coordinate, starting from upper border */
    uint32_t y;
} snk_position;

/**
 * Advance a position towards @p direction by one unit.
 *
 * @param[in,out] position  Position to advance
 * @param[in] direction     Advance direction
 *
 * @return                  Status code
 * @retval  SNK_RC_INVALID  Direction is invalid
 */
snk_rc_type snk_position_advance(snk_position *position, snk_direction direction);

/**
 * Compare two positions.
 *
 * @param[in] a     First position
 * @param[in] b     Second position
 *
 * @retval 1        Positions are equal
 * @retval 0        Positions are different
 */
int snk_position_compare(const snk_position *a, const snk_position *b);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_POSITION_H__ */
