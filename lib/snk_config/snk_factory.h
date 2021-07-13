#ifndef __SNK_FACTORY_H__
#define __SNK_FACTORY_H__

#include "snk.h"
#include "snk_config.h"

#ifdef __cplusplus
extern "C" {
#endif

snk_rc_type snk_create_from_config(const snk_config *config,
                                   snk_process *process);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __SNK_FACTORY_H__ */
