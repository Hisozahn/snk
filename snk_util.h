#ifndef __SNK_UTIL_H__
#define __SNK_UTIL_H__

#include "snk_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Get number of elements in an array */
#define SNK_ARRAY_LEN(_array)    (sizeof(_array) / sizeof((_array)[0]))
/** Maximum number that can be produced by snk_rand */
#define SNK_RAND_MAX    32767

/**
 * Produce pseudo-random number
 *
 * @param[in,out] seed  Random seed
 * @return              Random number
 */
uint32_t snk_rand(uint32_t *seed);

/**
 * Get opposite direction.
 *
 * @param[in] direction     Original direction
 *
 * @return                  Opposite direction
 */
snk_direction
snk_direction_reverse(snk_direction direction);

int
snk_direction_is_valid(snk_direction direction);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_UTIL_H__ */
