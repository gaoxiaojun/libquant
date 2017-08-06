/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#ifndef __CQUANT_EVENT_H__
#define __CQUANT_EVENT_H__

#include <stdarg.h>
#include <stdatomic.h>
#include "quant_config.h"
#include "quant_datetime.h"

G_BEGIN_DECLS

typedef enum {
  QUANT_EVENT_TYPE_EXTERNAL     = 1 << 0,
  QUANT_EVENT_TYPE_MARKET       = 1 << 1,
  QUANT_EVENT_TYPE_TICK         = 1 << 2,
  QUANT_EVENT_TYPE_EXECUTION    = 1 << 3,
  QUANT_EVENT_TYPE_SERVICE      = 1 << 4,
  QUANT_EVENT_TYPE_REMINDER     = 1 << 5
} QuantEventTypeFLags;

#define QUANT_EVENT_NUM_SHIFT (8)

#define QUANT_EVENT_MAKE_TYPE(num, flags) (((num) << QUANT_EVENT_NUM_SHIFT) | (flags))

#define FLAG(name) QUANT_EVENT_TYPE_##name

typedef enum {
  QUANT_EVENT_UNKNOW        = QUANT_EVENT_MAKE_TYPE(0, 0),

  /* reminder event */
  QUANT_EVENT_REMINDER      = QUANT_EVENT_MAKE_TYPE(10, FLAG(REMINDER)),
  /* market tick event */
  QUANT_EVENT_ASK           = QUANT_EVENT_MAKE_TYPE(20, FLAG(EXTERNAL) | FLAG(MARKET) | FLAG(TICK)),
  QUANT_EVENT_BID           = QUANT_EVENT_MAKE_TYPE(30, FLAG(EXTERNAL) | FLAG(MARKET) | FLAG(TICK)),
  QUANT_EVENT_TRADE         = QUANT_EVENT_MAKE_TYPE(40, FLAG(EXTERNAL) | FLAG(MARKET) | FLAG(TICK)),
  /* market event */
  QUANT_EVENT_BAR           = QUANT_EVENT_MAKE_TYPE(50, FLAG(EXTERNAL) | FLAG(MARKET)),
  QUANT_EVENT_NEWS          = QUANT_EVENT_MAKE_TYPE(60, FLAG(EXTERNAL) | FLAG(MARKET)),
  QUANT_EVENT_FUNDAMENTAL   = QUANT_EVENT_MAKE_TYPE(70, FLAG(EXTERNAL) | FLAG(MARKET)),

  /* execution event */

} QuantEventType;
#undef FLAG

#define QUANT_EVENT_CAST(obj)           ((QuantEvent *)(obj))
#define QUANT_EVENT(obj)                (QUANT_EVENT_CAST(obj))

#define QUANT_EVENT_TYPE(event)         (QUANT_EVENT_CAST(event)->type)
#define QUANT_EVENT_TIMESTAMP(event)    (QUANT_EVENT_CAST(event)->timestamp)
#define QUANT_EVENT_SEQNUM(event)       (QUANT_EVENT_CAST(event)->seqnum)

#define QUANT_EVENT_IS_EXTERNAL(event)  !!(QUANT_EVENT_TYPE(event) & QUANT_EVENT_TYPE_EXTERNAL)
#define QUANT_EVENT_IS_MARKET(event)    !!(QUANT_EVENT_TYPE(event) & QUANT_EVENT_TYPE_MARKET)
#define QUANT_EVENT_IS_TICK(event)      !!(QUANT_EVENT_TYPE(event) & QUANT_EVENT_TYPE_TICK)
#define QUANT_EVENT_IS_EXECUTION(event) !!(QUANT_EVENT_TYPE(event) & QUANT_EVENT_TYPE_EXECUTION)
#define QUANT_EVENT_IS_SERVICE(event)   !!(QUANT_EVENT_TYPE(event) & QUANT_EVENT_TYPE_SERVICE)
#define QUANT_EVENT_IS_REMINDER(event)  !!(QUANT_EVENT_TYPE(event) & QUANT_EVENT_TYPE_REMINDER)


typedef  struct _QuantEvent QuantEvent;

struct _QuantEvent {
  gint refcount;
  QuantEventType type;
  datetime_t timestamp;
};

static inline QuantEventTypeFLags quant_event_type_get_flags(QuantEventType type)
{
    QuantEventTypeFLags ret;
    ret = type & ((1 << QUANT_EVENT_NUM_SHIFT) - 1);
    return ret;
}

#define QUANT_EVENT_REFCOUNT(obj)           ((QUANT_EVENT_CAST(obj))->refcount)
#define QUANT_EVENT_REFCOUNT_VALUE(obj)     (g_atomic_int_get(&(QUANT_EVENT_CAST(obj))->refcount))

QuantEvent *
quant_event_new(QuantEventType type);

void quant_event_init(QuantEvent *event);

void quant_event_free(QuantEvent *event);

static inline QuantEvent *quant_event_ref(QuantEvent *event) {
  g_return_val_if_fail(event != NULL, NULL);
  g_atomic_int_inc(&event->refcount);
  return event;
}

static inline
void quant_event_unref(QuantEvent *event)
{
   g_return_if_fail(event != NULL);
   if(g_atomic_int_dec_and_test(&event->refcount))
       quant_event_free(event);
}

QuantEvent *quant_event_copy(const QuantEvent *event);

G_END_DECLS

#endif // __CQUANT_EVENT_H__
