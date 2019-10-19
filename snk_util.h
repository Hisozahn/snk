#ifndef __SNK_UTIL_H__
#define __SNK_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SNK_ARRAY_LEN(_array)    (sizeof(_array) / sizeof((_array)[0]))
#define SNK_RAND_MAX    32767

static inline uint32_t snk_rand(uint32_t *seed)
{
    *seed = ((*seed * 1103515245 + 12345) / 65536) % SNK_RAND_MAX;

    return *seed;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_UTIL_H__ */
