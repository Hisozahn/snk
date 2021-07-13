#ifndef __SNK_CONFIG_H__
#define __SNK_CONFIG_H__

#include "snk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SNK_CONFIG_RAND_SEED_UNSET 0

typedef struct snk_field_config {
    uint32_t width; /**< Field's width */
    uint32_t height; /**< Field's height */

    snk_field_obstacle obstacles[SNK_FIELD_OBSTACLE_MAX]; /**< Field's obstacle collection */
    uint32_t n_obstacles; /**< Number of obstacles */
} snk_field_config;

typedef struct snk_snakes_config {
    snk_position start_positions[SNK_SNAKES_MAX]; /**< Start positions of the snakes */
    snk_direction start_directions[SNK_SNAKES_MAX]; /**< Start directions of the snakes */
    uint32_t start_lengths[SNK_SNAKES_MAX]; /**< Start lenghts of the snakes */
    uint32_t n_snakes; /**< Count of the snakes */
} snk_snakes_config;

typedef struct snk_config {
    snk_settings settings; /**< Gameplay settings */
    snk_field_config field; /**< Field setup */
    snk_snakes_config snakes; /**< Snakes setup */
} snk_config;

snk_rc_type snk_config_load_from_json(const char *json_str, snk_config *config);

/**
 * Set random seed for the snk process
 * when it is unset (#SNK_CONFIG_RAND_SEED_UNSET).
 * When random seed is unset, the user can provide its own generated seed value.
 *
 *
 * @param config        Configuration
 * @param rand_seed     Seed to set
 */
void snk_config_set_seed_if_unset(snk_config *config, uint32_t rand_seed);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_CONFIG_H__ */
