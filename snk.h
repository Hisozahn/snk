#ifndef __SNK_H__
#define __SNK_H__

#include <stdint.h>
#include <stddef.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SNK_FIELD_OBSTACLE_MAX 32

/** Direction of an object */
typedef enum snk_direction {
    SNK_LEFT,
    SNK_UP,
    SNK_RIGHT,
    SNK_DOWN,
} snk_direction;

/** Position on a field */
typedef struct snk_position {
    /** Horizontal coordinate, starting from left border */
    int8_t x;
    /** Vertical coordinate, starting from upper border */
    int8_t y;
} snk_position;

/** Field obstacle */
typedef struct snk_field_obstacle {
    snk_position top_left;
    snk_position bottom_right;
} snk_field_obstacle;

/** Field dimensions */
typedef struct snk_field {
    uint8_t width;
    uint8_t height;
    snk_field_obstacle obstacles[SNK_FIELD_OBSTACLE_MAX];
    uint8_t n_obstacles;
} snk_field;

typedef struct snk_process snk_process;

int snk_position_advance(snk_position *position, snk_direction direction);
int snk_position_compare(const snk_position *a, const snk_position *b);
int snk_create_field(uint8_t width, uint8_t height, uint8_t n_obstacles, const snk_field_obstacle *obstacles, snk_field *field);
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
