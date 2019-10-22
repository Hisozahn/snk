#ifndef __SNK_UTIL_H__
#define __SNK_UTIL_H__

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
static inline uint32_t snk_rand(uint32_t *seed)
{
    *seed = ((*seed * 1103515245 + 12345) / 65536) % SNK_RAND_MAX;

    return *seed;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_UTIL_H__ */
