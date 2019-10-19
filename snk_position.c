#include "snk_position.h"

snk_rc_type
snk_position_advance(snk_position *position, snk_direction direction)
{
    switch (direction)
    {
        case SNK_DIRECTION_LEFT:
            position->x--;
            break;
        case SNK_DIRECTION_RIGHT:
            position->x++;
            break;
        case SNK_DIRECTION_UP:
            position->y--;
            break;
        case SNK_DIRECTION_DOWN:
            position->y++;
            break;
        default:
            return SNK_RC_INVALID;
    }

    return SNK_RC_SUCCESS;
}

int
snk_position_compare(const snk_position *a, const snk_position *b)
{
    return !(a->y == b->y && a->x == b->x);
}