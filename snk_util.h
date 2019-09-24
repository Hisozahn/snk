#ifndef __SNK_UTIL_H__
#define __SNK_UTIL_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SNK_ARRAY_LEN(_array)    (sizeof(_array) / sizeof(_array[0]))

void snk_assert(int expr)
{
    if (!expr)
        fprintf(stderr, "assert failed\n");
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_UTIL_H__ */
