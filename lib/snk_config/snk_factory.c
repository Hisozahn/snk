#include "snk_factory.h"

snk_rc_type
snk_create_from_config(const snk_config *config, snk_process *process)
{
    const snk_field_config *field_config = &config->field;
    const snk_snakes_config *snakes = &config->snakes;
    snk_field field;
    snk_rc_type rc;

    rc = snk_create_field(field_config->width, field_config->height,
                          field_config->n_obstacles, field_config->obstacles,
                          &field);
    if (rc != 0) {
        return rc;
    }

    return snk_create(&field, snakes->n_snakes, snakes->start_positions,
                      snakes->start_directions, snakes->start_lengths,
                      &config->settings, process);
}
