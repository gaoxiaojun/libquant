/******************************************************************************
 * Crazy Quantitative Library                                                 *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#ifndef QUANT_EVENT_BUS_H
#define QUANT_EVENT_BUS_H

#include <glib.h>
#include <glib-object.h>
#include "quant_event.h"
#include "quant_mpmc_queue.h"
#include "quant_event_queue.h"
#include "quant_event_loop.h"

G_BEGIN_DECLS

typedef struct _QuantEventBus QuantEventBus;

typedef enum {
    QUANT_EVENT_BUS_SIMUALTOR_MODE,
    QUANT_EVENT_BUS_REALTIME_MODE
}QuantEventBusMode;

QUANT_EXPORT
QuantEventBus *quant_event_bus_new(QuantEventLoop *loop) G_GNUC_MALLOC;

QUANT_EXPORT
QuantEventBus* quant_event_bus_ref(QuantEventBus *bus);

QUANT_EXPORT
void quant_event_bus_unref(QuantEventBus *bus);

QUANT_EXPORT
int quant_event_bus_push(QuantEventBus *bus, QuantEvent *e);


QUANT_EXPORT
QuantEventBusMode quant_event_bus_get_mode(QuantEventBus *bus);

QUANT_EXPORT
void quant_event_bus_set_mode(QuantEventBus *bus, QuantEventBusMode mode);


QUANT_EXPORT
datetime_t quant_event_bus_get_datetime(QuantEventBus *bus);

QUANT_EXPORT
void quant_event_bus_set_datetime(QuantEventBus *bus, datetime_t time);

QUANT_EXPORT
datetime_t quant_event_bus_get_exchange_datetime(QuantEventBus *bus);

QUANT_EXPORT
void quant_event_bus_set_exchange_datetime(QuantEventBus *bus, datetime_t time);

QUANT_EXPORT
QuantEvent * quant_event_bus_next_event(QuantEventBus *bus);

QUANT_EXPORT
timespan_t quant_event_bus_next_timeout(QuantEventBus *bus);

G_END_DECLS

#endif // QUANT_EVENT_BUS_H
