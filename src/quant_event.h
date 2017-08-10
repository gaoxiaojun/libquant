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

QUANT_BEGIN_DECLS

typedef enum {
  QUANT_EVENT_FLAG_EXTERNAL     = 1 << 0,
  QUANT_EVENT_FLAG_MARKET       = 1 << 1,
  QUANT_EVENT_FLAG_TICK         = 1 << 2,
  QUANT_EVENT_FLAG_EXECUTION    = 1 << 3,
  QUANT_EVENT_FLAG_SERVICE      = 1 << 4,
  QUANT_EVENT_FLAG_REMINDER     = 1 << 5
} QuantEventFlags;

#define QUANT_DEF_FLAG(name) QUANT_EVENT_FLAG_##name

#define QUANT_FLAG_TICK     QUANT_DEF_FLAG(EXTERNAL) | QUANT_DEF_FLAG(MARKET) | QUANT_DEF_FLAG(TICK)
#define QUANT_FLAG_MARKET   QUANT_DEF_FLAG(EXTERNAL) | QUANT_DEF_FLAG(MARKET)
#define QUANT_FLAG_REMINDER QUANT_DEF_FLAG(REMINDER)

#define QUANT_EVENT_DEFINE(_)                                                         \
_(REMINDER,     reminder,       QUANT_FLAG_REMINDER,    ___, ___) \
_(ASK,          ask,            QUANT_FLAG_TICK,        ___, ___) \
_(BID,          bid,            QUANT_FLAG_TICK,        ___, ___) \
_(TRADE,        trade,          QUANT_FLAG_TICK,        ___, ___)

typedef enum {
  QUANT_EVENT_UNKNOW,

  /* reminder event */
  QUANT_EVENT_REMINDER,
  /* market tick event */
  QUANT_EVENT_ASK,
  QUANT_EVENT_BID,
  QUANT_EVENT_TRADE,
  /* market event */
  QUANT_EVENT_BAR,
  QUANT_EVENT_NEWS,
  QUANT_EVENT_FUNDAMENTAL,

  /* execution event */

    /* custom event */
  QUANT_EVENT_CUSTOM_START,
    /* last */
  QUANT_EVENT_LAST
} QuantEventType;


#define QUANT_EVENT_CAST(obj)           ((QuantEvent *)(obj))
#define QUANT_EVENT(obj)                (QUANT_EVENT_CAST(obj))

#define QUANT_EVENT_TYPE(event)         ((QuantEventType)(QUANT_EVENT_CAST(event)->type))
#define QUANT_EVENT_FLAG(event)         (QUANT_EVENT_CAST(event)->flag)
#define QUANT_EVENT_TIMESTAMP(event)    (QUANT_EVENT_CAST(event)->timestamp)

#define QUANT_EVENT_IS_EXTERNAL(event)  !!(QUANT_EVENT_FLAG(event) & QUANT_DEF_FLAG(EXTERNAL))
#define QUANT_EVENT_IS_MARKET(event)    !!(QUANT_EVENT_FLAG(event) & QUANT_DEF_FLAG(MARKET))
#define QUANT_EVENT_IS_TICK(event)      !!(QUANT_EVENT_FLAG(event) & QUANT_DEF_FLAG(TICK))
#define QUANT_EVENT_IS_EXECUTION(event) !!(QUANT_EVENT_FLAG(event) & QUANT_DEF_FLAG(EXECUTION))
#define QUANT_EVENT_IS_SERVICE(event)   !!(QUANT_EVENT_FLAG(event) & QUANT_DEF_FLAG(SERVICE))
#define QUANT_EVENT_IS_REMINDER(event)  !!(QUANT_EVENT_FLAG(event) & QUANT_DEF_FLAG(REMINDER))

#define QUANT_EVENT_REF_INIT(event)     (QUANT_EVENT_CAST(event)->refcount = 1)
#define QUANT_EVENT_REF(event)          (quant_event_ref(QUANT_EVENT_CAST(event)))
#define QUANT_EVENT_UNREF(event)        (quant_event_unref(QUANT_EVENT_CAST(event)))

//#undef FLAG

typedef  struct _QuantEvent QuantEvent;

struct _QuantEvent {
  gint refcount;
  QuantEventType type;
  QuantEventFlags flag;
  datetime_t timestamp;
};

typedef void (*quant_event_init)(QuantEvent* e, va_list args);
typedef void (*quant_event_destory)(QuantEvent* e);
typedef QuantEvent* (*quant_event_clone)(QuantEvent* e);
typedef void (*quant_event_default_process)(QuantEvent* e);

typedef struct _QuantEventClassInfo {
    size_t size;
    quant_event_init init;
    quant_event_destory destory;
    quant_event_clone clone;
    quant_event_default_process processor;
} QuantEventClassInfo;

extern QuantEventClassInfo _eventclassinfo[QUANT_EVENT_LAST];

QUANT_EXPORT QuantEvent *
quant_event_malloc(QuantEventType type);

QUANT_EXPORT QuantEvent *
quant_event_new(QuantEventType type, ...);

QUANT_EXPORT QuantEvent *
quant_event_new_with_time(QuantEventType type, datetime_t time, ...);

QUANT_EXPORT QuantEvent *quant_event_ref(QuantEvent *event);

QUANT_EXPORT void quant_event_unref(QuantEvent *event);

QUANT_EXPORT QuantEvent *quant_event_copy(QuantEvent *event);

QUANT_FUNC void quant_event_free(QuantEvent *e);

QUANT_END_DECLS

#endif // __CQUANT_EVENT_H__
