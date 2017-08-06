/******************************************************************************
 * Crazy Quantitative Library                                                 *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#ifndef QUANT_LOOP_H
#define QUANT_LOOP_H

#include <glib.h>
#include <glib-object.h>
#include "quant_config.h"

G_BEGIN_DECLS

typedef struct _QuantEventLoop QuantEventLoop;

QUANT_EXPORT
QuantEventLoop *quant_event_loop_new() G_GNUC_MALLOC;

QUANT_EXPORT
void quant_event_loop_ref(QuantEventLoop *loop);

QUANT_EXPORT
void quant_event_loop_unref(QuantEventLoop *loop);

G_END_DECLS

#endif // QUANT_LOOP_H
