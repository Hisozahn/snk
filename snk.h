#ifndef __SNK_H__
#define __SNK_H__

#include <stdint.h>
#include <stddef.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Field dimentions */
typedef struct snk_field {
    uint8_t width;
    uint8_t height;
} snk_field;

/** Direction of an object */
typedef enum snk_direction {
    SNK_LEFT,
    SNK_UP,
    SNK_RIGHT,
    SNK_DOWN,
} snk_direction;

/** Position on a field */
typedef struct snk_position {
    /** Horisontal coordinate, starting from left border */
    int8_t x;
    /** Vertical coordinate, starting from upper border */
    int8_t y;
} snk_position;

typedef struct snk_process snk_process;

int snk_create(const snk_field *field, const snk_position *start_position,
               const snk_direction *start_direction, uint16_t start_length, snk_process *process);
int snk_start(snk_process *process);
int snk_next_tick(snk_process *process);
int snk_choose_direction(snk_process *process, snk_direction direction);
int snk_destroy(snk_process *process);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_H__ */
