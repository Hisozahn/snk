#ifndef __SNK_H__
#define __SNK_H__

#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include "snk_defs.h"
#include "snk_snake.h"
#include "snk_joint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SNK_FIELD_OBSTACLE_MAX 32

/** Field obstacle */
typedef struct snk_field_obstacle {
    snk_position top_left;
    snk_position bottom_right;
} snk_field_obstacle;

/** Field dimensions */
typedef struct snk_field {
    uint32_t width;
    uint32_t height;
    snk_field_obstacle obstacles[SNK_FIELD_OBSTACLE_MAX];
    uint32_t n_obstacles;
    snk_position food;
    uint32_t n_food;
    uint32_t rand_seed;
} snk_field;

typedef struct snk_process {
    snk_field field;
    snk_snake snake;
    snk_direction next_direction;
} snk_process;

int snk_position_advance(snk_position *position, snk_direction direction);
int snk_position_compare(const snk_position *a, const snk_position *b);

int snk_create_field(uint32_t width, uint32_t height, uint32_t n_obstacles, const snk_field_obstacle *obstacles,
                     uint32_t rand_seed,  snk_field *field);
int snk_create(const snk_field *field, const snk_position *start_position,
               snk_direction start_direction, uint32_t start_length, snk_process *process);
int snk_next_tick(snk_process *process);
int snk_choose_direction(snk_process *process, snk_direction direction);
int snk_render(const snk_process *process, uint8_t *data, size_t data_size);
int snk_destroy(snk_process *process);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_H__ */
