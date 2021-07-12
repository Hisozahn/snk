#ifndef __SNK_CONFIG_H__
#define __SNK_CONFIG_H__

#include "snk.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct snk_config {
    snk_settings settings;

    uint32_t width; /**< Field's width */
    uint32_t height; /**< Field's height */

    snk_field_obstacle obstacles[SNK_FIELD_OBSTACLE_MAX]; /**< Field's obstacle collection */
    uint32_t n_obstacles; /**< Number of obstacles */

    snk_position start_positions[SNK_SNAKES_MAX];
    snk_direction start_directions[SNK_SNAKES_MAX];
    uint32_t start_lengths[SNK_SNAKES_MAX];
    uint32_t n_snakes;
} snk_config;

snk_rc_type snk_config_load_from_json(const char *json_str, snk_config *config);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_CONFIG_H__ */
