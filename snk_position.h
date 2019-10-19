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

snk_rc_type snk_position_advance(snk_position *position, snk_direction direction);
int snk_position_compare(const snk_position *a, const snk_position *b);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_POSITION_H__ */
