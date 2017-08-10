/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include "quant_event.h"
#include "quant_event_market.h"
#include "quant_event_reminder.h"
#include <stdarg.h>
#include <assert.h>
/*
unsigned short event_ref(QuantEvent* e)
{
    return atomic_fetch_add(&e->refcount, 1);
}

unsigned short event_unref(QuantEvent* e)
{
    unsigned short count;
    count = atomic_fetch_sub(&e->refcount, 1);
    if (count == 1) {
        zq_event_free(e);
    }
    return count;
}

int event_is_single_ref(QuantEvent* e)
{
    return (atomic_load(&e->refcount) == 1);
}
*/
static inline int event_check_type(QuantEventType type)
{
    if (type >= QUANT_EVENT_CUSTOM_START)
        return 1;
    return 0;
}

void
quant_event_free(QuantEvent *e)
{
    quant_event_destory destory = _eventclassinfo[e->type].destory;
    if (destory)
        destory(e);
    free(e);
}

QuantEvent *quant_event_copy(QuantEvent *e)
{
    QuantEvent *result;
    quant_event_clone clone = _eventclassinfo[e->type].clone;
    if(clone)
        result = clone(e);
    else {
        size_t size = _eventclassinfo[e->type].size;
        result = malloc(size);
        memcpy(result, e, size);
    }
    return result;
}

QuantEvent *quant_event_ref(QuantEvent *event)
{
  g_return_val_if_fail(event != NULL, NULL);
  g_atomic_int_inc(&event->refcount);
  return event;
}

void quant_event_unref(QuantEvent *event)
{
   g_return_if_fail(event != NULL);
   if(g_atomic_int_dec_and_test(&event->refcount))
       quant_event_free(event);
}

QuantEvent* quant_event_malloc(QuantEventType type)
{
    assert(!event_check_type(type));
    QuantEvent* e = malloc(_eventclassinfo[type].size);
    if (e) {
        e->type = type;
        e->refcount = 1;
    }
    return e;
}


QuantEvent *quant_event_new(QuantEventType type, ...)
{
    assert(!event_check_type(type));
    QuantEvent* e = quant_event_malloc(type);

    va_list args;
    va_start(args, type);
    quant_event_init init = _eventclassinfo[type].init;
    if (init)
        init(e, args);
    va_end(args);

    return e;
}

QuantEventClassInfo _eventclassinfo[QUANT_EVENT_LAST] = {
    {// REMINDER
        .init = (quant_event_init)quant_event_reminder_init,
        .destory = NULL,
        .clone = NULL,
        .size = sizeof(QuantReminderEvent)
    },
    { // ASK
        .init = (quant_event_init)quant_event_tick_init,
        .destory = NULL,
        .clone = NULL,
        .size = sizeof(QuantAskEvent)
    },
    { // BID
        .init = (quant_event_init)quant_event_tick_init,
        .destory = NULL,
        .clone = NULL,
        .size = sizeof(QuantBidEvent)
    },
    { // TRADE
        .init = (quant_event_init)quant_event_tick_init,
        .destory = NULL,
        .clone = NULL,
        .size = sizeof(QuantTradeEvent)
    },
};
