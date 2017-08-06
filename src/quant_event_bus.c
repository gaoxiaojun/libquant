/******************************************************************************
 * Crazy Quantitative Library                                                 *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 ******************************************************************************/

#include "quant_event_bus.h"
#include "quant_event_queue.h"
#include "quant_event_reminder.h"

struct _QuantEventBus {
    gint refcount;
    QuantEventLoop *loop;
    QuantEventBusMode mode;
    QuantEventQueue *queue[6];
    guint64 clock_datetime[2];

    QuantEvent *saved_event;
};

typedef enum {
    QUANT_EVENT_BUS_MARKET,
    QUANT_EVENT_BUS_EXECUTION,
    QUANT_EVENT_BUS_SERVICE,
    QUANT_EVENT_BUS_COMMAND,
    QUANT_EVENT_BUS_LOCAL_CLOCK,
    QUANT_EVENT_BUS_EXCHANGE_CLOCK
}QuantEventBusPipe;

//TODO:event lifetime

#define LOG_OUT_OF_ORDER(t, e) g_log("EVENTBUS", G_LOG_LEVEL_ERROR, "out of time order")
static QuantEvent* simulator_dequeue(QuantEventBus* bus)
{
again:
    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_MARKET]) && bus->saved_event == NULL) {
        QuantEvent* e = quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_MARKET]);

        datetime_t bus_time = quant_event_bus_get_datetime(bus);
        if (QUANT_EVENT_TIMESTAMP(e) < bus_time) {
            LOG_OUT_OF_ORDER(bus_time, e);
            goto again;
        }
        bus->saved_event = e;
    }

    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_EXECUTION])) {
        return quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_EXECUTION]);
    }

    // local clock
    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_LOCAL_CLOCK])) {
        if (bus->saved_event) {
            QuantReminderEvent* r = (QuantReminderEvent*)quant_event_queue_peek(bus->queue[QUANT_EVENT_BUS_LOCAL_CLOCK]);
            if (QUANT_EVENT_TIMESTAMP(r) <= QUANT_EVENT_TIMESTAMP(bus->saved_event)) {
                return quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_LOCAL_CLOCK]);
            }
        }
    }

    // exchage clock
    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_EXCHANGE_CLOCK])
        && bus->saved_event != NULL && QUANT_EVENT_IS_TICK(bus->saved_event)) {
        QuantReminderEvent* r = (QuantReminderEvent*)quant_event_queue_peek(bus->queue[QUANT_EVENT_BUS_EXCHANGE_CLOCK]);
        if (QUANT_EVENT_TIMESTAMP(r) <= QUANT_EVENT_TIMESTAMP(bus->saved_event)) {
            return quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_EXCHANGE_CLOCK]);
        }
    }

    // service
    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_SERVICE])) {
        return quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_SERVICE]);
    }

    // command
    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_COMMAND])) {
        return quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_COMMAND]);
    }

    if (bus->saved_event) {
        QuantEvent* e = bus->saved_event;
        bus->saved_event = NULL;
        return e;
    }

    return NULL;
}

static QuantEvent* realtime_dequeue(QuantEventBus* bus)
{
    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_MARKET]) && bus->saved_event == NULL) {
        bus->saved_event = quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_MARKET]);
    }

    // local clock
    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_LOCAL_CLOCK])) {
        QuantReminderEvent* r = (QuantReminderEvent*)quant_event_queue_peek(bus->queue[QUANT_EVENT_BUS_LOCAL_CLOCK]);
        if (QUANT_EVENT_TIMESTAMP(r) <= quant_event_bus_get_datetime(bus)) {
            return quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_LOCAL_CLOCK]);
        }
    }

    // exchage clock
    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_EXCHANGE_CLOCK]) && bus->saved_event != NULL
        && QUANT_EVENT_IS_TICK(bus->saved_event)) {
        QuantReminderEvent* r = (QuantReminderEvent*)quant_event_queue_peek(bus->queue[QUANT_EVENT_BUS_EXCHANGE_CLOCK]);
        if (QUANT_EVENT_TIMESTAMP(r) <= QUANT_EVENT_TIMESTAMP(bus->saved_event)) {
            return quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_EXCHANGE_CLOCK]);
        }
    }

    // execution
    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_EXECUTION])) {
        return quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_EXECUTION]);
    }

    // service
    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_SERVICE])) {
        return quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_SERVICE]);
    }

    // command
    if (!quant_event_queue_is_empty(bus->queue[QUANT_EVENT_BUS_COMMAND])) {
        return quant_event_queue_pop(bus->queue[QUANT_EVENT_BUS_COMMAND]);
    }

    if (bus->saved_event) {
        QuantEvent* e = bus->saved_event;
        bus->saved_event = NULL;
        return e;
    }

    return NULL;
}

static
void quant_event_bus_free(QuantEventBus *bus)
{
    for (int i = 0; i < 6; i++) {
        if (bus->queue[i] != NULL)
            quant_event_queue_unref(bus->queue[i]);
    }

    if (bus->saved_event)
        quant_event_unref(bus->saved_event);

    if(bus)
        g_free(bus);
}

QuantEventBus *quant_event_bus_new(QuantEventLoop *loop)
{
    QuantEventBus * bus = g_new0(QuantEventBus, 1);
    if (bus != NULL) {
        for (int i = 0; i < 6; i++) {
            bus->queue[i] = quant_event_queue_new();
            if (bus->queue[i] == NULL)
                goto bad_alloc;
        }
        bus->loop = loop;
    }
    return bus;
bad_alloc:
    quant_event_bus_free(bus);

    return NULL;
}

QuantEventBus *quant_event_bus_ref(QuantEventBus *bus)
{
    g_return_val_if_fail(bus != NULL, NULL);
    g_atomic_int_inc(&bus->refcount);
    return bus;
}

void quant_event_bus_unref(QuantEventBus *bus)
{
    g_return_if_fail(bus != NULL);
    if (g_atomic_int_dec_and_test(&bus->refcount))
        quant_event_bus_free(bus);
}

static inline QuantEventBusPipe event_type_to_pipe(QuantEvent *e)
{
    if(QUANT_EVENT_IS_REMINDER(e)) {
        QuantReminderEvent *r = (QuantReminderEvent*)e;
        if (r->clock_type == QUANT_REMINDER_EVENT_LOCAL_CLOCK)
            return QUANT_EVENT_BUS_LOCAL_CLOCK;
        else
            return QUANT_EVENT_BUS_EXCHANGE_CLOCK;
    }
    if(QUANT_EVENT_IS_MARKET(e))
        return QUANT_EVENT_BUS_MARKET;
    if(QUANT_EVENT_IS_EXECUTION(e))
        return QUANT_EVENT_BUS_EXECUTION;
    if(QUANT_EVENT_IS_SERVICE(e))
        return QUANT_EVENT_BUS_SERVICE;
    return QUANT_EVENT_BUS_COMMAND;
}

int quant_event_bus_push(QuantEventBus *bus, QuantEvent *e)
{
    return quant_event_queue_push(bus->queue[event_type_to_pipe(e)], e);
}

QuantEventBusMode quant_event_bus_get_mode(QuantEventBus *bus)
{
    return bus->mode;
}


void quant_event_bus_set_mode(QuantEventBus *bus, QuantEventBusMode mode)
{
    //TODO:
    bus->mode = mode;
}



datetime_t quant_event_bus_get_datetime(QuantEventBus *bus)
{
    return bus->clock_datetime[0];
}


void quant_event_bus_set_datetime(QuantEventBus *bus, datetime_t time)
{
    if (time < bus->clock_datetime[0]) {
        LOG_OUT_OF_ORDER(bus, e);
        return;
    }
    bus->clock_datetime[0] = time;
}


datetime_t quant_event_bus_get_exchange_datetime(QuantEventBus *bus)
{
    return bus->clock_datetime[1];
}


void quant_event_bus_set_exchange_datetime(QuantEventBus *bus, datetime_t time)
{
    if (time < bus->clock_datetime[1]) {
        LOG_OUT_OF_ORDER(bus, e);
        return;
    }
    bus->clock_datetime[1] = time;
}


QuantEvent * quant_event_bus_pop(QuantEventBus *bus)
{
    if (bus->mode == QUANT_EVENT_BUS_SIMUALTOR_MODE)
        return simulator_dequeue(bus);
    else
        return realtime_dequeue(bus);
}


timespan_t quant_event_bus_next_timeout(QuantEventBus *bus)
{
    QuantReminderEvent *r = (QuantReminderEvent*)quant_event_queue_peek(bus->queue[QUANT_EVENT_BUS_LOCAL_CLOCK]);
    if (r)
        return QUANT_EVENT_TIMESTAMP(r) - datetime_now();
    else
        return 0;
}
