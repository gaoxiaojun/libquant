/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#ifndef QUANT_FRAMEWORK_H
#define QUANT_FRAMEWORK_H

#include "quant_config.h"
#include "quant_event.h"

QUANT_BEGIN_DECLS

typedef struct _QuantContext QuantContext;

QUANT_EXPORT
QuantContext *quant_context_new();

QUANT_EXPORT
QuantContext *quant_context_ref(QuantContext *ctx);

QUANT_EXPORT
QuantContext *quant_context_unref(QuantContext *ctx);

typedef enum {
    QUANT_CONTEXT_MODE_SIMULATOR,
    QUANT_CONTEXT_MODE_REALTIME
}QuantContextMode;

typedef enum {
    QUANT_CONTEXT_STATUS_STARTED,
    QUANT_CONTEXT_STATUS_STOPPED,
    QUANT_CONTEXT_STATUS_STEPING,
    QUANT_CONTEXT_STATUS_PAUSED
}QuantContextStatus;

QUANT_EXPORT
QuantContextMode *quant_context_get_mode(QuantContext *ctx);

QUANT_EXPORT
QuantContextStatus quant_context_get_statue(QuantContext *ctx);

QUANT_EXPORT
void quant_context_set_mode(QuantContext *ctx, QuantContextMode *mode);

QUANT_EXPORT
int quant_context_start(QuantContext *ctx);

QUANT_EXPORT
int quant_context_stop(QuantContext *ctx);

QUANT_EXPORT
int quant_context_step(QuantContext *ctx, QuantEventType stop_on_type);

QUANT_EXPORT
int quant_context_pause(QuantContext *ctx);

QUANT_EXPORT
int quant_context_pause_with_timeout(QuantContext *ctx, int microseconds);



QUANT_END_DECLS

#endif // QUANT_FRAMEWORK_H
