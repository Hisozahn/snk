#include <jansson.h>
#include <string.h>

#include "snk_config.h"

#define JSON_VALUE(_object, _type, _key, _result)    \
    do {                                             \
        json_t *obj;                                 \
                                                     \
        obj = json_object_get((_object), (_key));    \
        (_result) = json_##_type##_value(obj);       \
    } while (0)

static snk_direction
snk_direction_by_string(const char *direction)
{
    if (strcmp(direction, "left") == 0)
        return SNK_DIRECTION_LEFT;
    else if (strcmp(direction, "up") == 0)
        return SNK_DIRECTION_UP;
    else if (strcmp(direction, "right") == 0)
        return SNK_DIRECTION_RIGHT;
    else if (strcmp(direction, "down") == 0)
        return SNK_DIRECTION_DOWN;

    return SNK_DIRECTION_INVALID;
}

static snk_rc_type
snk_load_position(const json_t *json_position, snk_position *position)
{
    json_int_t x;
    json_int_t y;

    JSON_VALUE(json_position, integer, "x", x);
    JSON_VALUE(json_position, integer, "y", y);

    if (x < 0 || y < 0)
        return SNK_RC_INVALID;

    position->x = x;
    position->y = y;

    return 0;
}

static snk_rc_type
snk_load_snake(json_t *snake, snk_position *position, snk_direction *direction,
               uint32_t *length)
{
    json_int_t len;
    const char *direction_str;
    snk_direction snake_direction;
    snk_position snake_position;
    json_t *json_postion;
    snk_rc_type rc;

    JSON_VALUE(snake, integer, "length", len);
    if (len < 0)
        return SNK_RC_INVALID;

    JSON_VALUE(snake, string, "start_direction", direction_str);
    snake_direction = snk_direction_by_string(direction_str);
    if (snake_direction == SNK_DIRECTION_INVALID)
        return SNK_RC_INVALID;

    json_postion = json_object_get(snake, "start_position");
    if (json_postion == NULL)
        return SNK_RC_INVALID;

    rc = snk_load_position(json_postion, &snake_position);
    if (rc != 0)
        return SNK_RC_INVALID;

    *length = len;
    *direction = snake_direction;
    *position = snake_position;

    fprintf(stderr, "%s: %d len: %lld\n", __func__, __LINE__, len);
    return 0;
}

static snk_rc_type
snk_load_obstacle(json_t *json, snk_field_obstacle *obstacle)
{
    snk_position top_left;
    json_int_t width;
    json_int_t height;
    snk_rc_type rc;

    rc = snk_load_position(json, &top_left);
    if (rc != 0) {
        return rc;
    }

    JSON_VALUE(json, integer, "width", width);
    JSON_VALUE(json, integer, "height", height);

    if (width < 1 || height < 1) {
        return SNK_RC_INVALID;
    }

    {
        snk_position bottom_right = {
            .x = top_left.x + width - 1,
            .y = top_left.y + height - 1
        };

        obstacle->top_left = top_left;
        obstacle->bottom_right = bottom_right;
    }

    return 0;
}

static snk_rc_type
snk_load_obstacles(json_t *json_obstacles, uint32_t *n_obstacles,
                   snk_field_obstacle *obstacles)
{
    snk_rc_type rc;
    json_int_t len;
    json_int_t i;

    len = json_array_size(json_obstacles);
    if (len < 0 || len > SNK_FIELD_OBSTACLE_MAX) {
        return SNK_RC_INVALID;
    }

    for (i = 0; i < len; i++) {
        rc = snk_load_obstacle(json_array_get(json_obstacles, i),
                               &obstacles[i]);
        if (rc != 0) {
            return rc;
        }
    }

    *n_obstacles = len;

    return 0;
}

static snk_rc_type
snk_load_field(json_t *field, snk_field_config *config)
{
    json_t *obstacles;

    JSON_VALUE(field, integer, "width", config->width);
    JSON_VALUE(field, integer, "height", config->height);

    obstacles = json_object_get(field, "obstacles");
    if (obstacles == NULL) {
        return SNK_RC_INVALID;
    }

    return snk_load_obstacles(obstacles, &config->n_obstacles,
                              config->obstacles);
}

static snk_rc_type
snk_load_settings(json_t *json_settings, snk_settings *settings)
{
    JSON_VALUE(json_settings, boolean, "wrap_joints", settings->wrap_joints);
    JSON_VALUE(json_settings, integer, "rand_seed", settings->rand_seed);

    return 0;
}

static snk_rc_type
snk_load_snakes(json_t *json_snakes, snk_snakes_config *config)
{
    size_t n_snakes;
    snk_rc_type rc;
    size_t i;

    n_snakes = json_array_size(json_snakes);
    if (n_snakes > SNK_SNAKES_MAX) {
        fprintf(stderr, "%s: %d error: too many snakes specified\n", __func__, __LINE__);
        return SNK_RC_INVALID;
    }

    for (i = 0; i < n_snakes; i++)
    {
        rc = snk_load_snake(json_array_get(json_snakes, i),
                            &config->start_positions[i],
                            &config->start_directions[i],
                            &config->start_lengths[i]);
        if (rc != 0)
            return rc;
    }

    config->n_snakes = n_snakes;

    return 0;
}

snk_rc_type
snk_config_load_from_json(const char *json_str, snk_config *config)
{
    json_error_t error;
    json_t *settings;
    json_t *field;
    json_t *snakes;
    snk_rc_type rc;
    json_t *json;

    json = json_loads(json_str, 0, &error);
    if (json == NULL)
    {
        fprintf(stderr, "%s: %d error: '%s'\n", __func__, __LINE__, error.text);
        return SNK_RC_INVALID;
    }

    settings = json_object_get(json, "settings");
    field = json_object_get(json, "field");
    snakes = json_object_get(json, "snakes");
    if (settings == NULL || field == NULL || snakes == NULL) {
        return SNK_RC_INVALID;
    }

    rc = snk_load_settings(settings, &config->settings);
    if (rc != 0) {
        return rc;
    }

    rc = snk_load_field(field, &config->field);
    if (rc != 0) {
        return rc;
    }

    rc = snk_load_snakes(snakes, &config->snakes);
    if (rc != 0) {
        return rc;
    }

    return 0;
}

void
snk_config_set_seed_if_unset(snk_config *config, uint32_t rand_seed)
{
    if (config->settings.rand_seed == SNK_CONFIG_RAND_SEED_UNSET) {
        config->settings.rand_seed = rand_seed;
    }
}
