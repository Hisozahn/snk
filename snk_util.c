#include "snk_util.h"

uint32_t snk_rand(uint32_t *seed)
{
    *seed = ((*seed * 1103515245 + 12345) / 65536) % SNK_RAND_MAX;

    return *seed;
}

/**
 * Get opposite direction.
 *
 * @param[in] direction     Original direction
 *
 * @return                  Opposite direction
 */
snk_direction
snk_direction_reverse(snk_direction direction)
{
    switch (direction)
    {
        case SNK_DIRECTION_RIGHT:
            return SNK_DIRECTION_LEFT;
        case SNK_DIRECTION_LEFT:
            return SNK_DIRECTION_RIGHT;
        case SNK_DIRECTION_DOWN:
            return SNK_DIRECTION_UP;
        case SNK_DIRECTION_UP:
            return SNK_DIRECTION_DOWN;
        default:
            return SNK_DIRECTION_INVALID;
    }
}

int
snk_direction_is_valid(snk_direction direction)
{
    return (direction >= 0 && direction < SNK_DIRECTION_ENUM_VALID_SIZE);
}
