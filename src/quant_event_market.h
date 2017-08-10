/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#ifndef QUANT_EVENT_MARKET_H
#define QUANT_EVENT_MARKET_H

#include "quant_event.h"

QUANT_BEGIN_DECLS

#define QUANT_TICK_EVENT_CAST(obj)              ((QuantTickEvent*)(obj))
#define QUANT_MARKET_EVENT_CAST(obj)            ((QuantMarketEvent*)(obj))
#define QUANT_EVENT_PROVIDER(event)             (QUANT_MARKET_EVENT_CAST(event)->provider)
#define QUANT_EVENT_INSTRUMENT(event)           (QUANT_MARKET_EVENT_CAST(event)->instrument)
#define QUANT_EVENT_EXCHANGE_TIMESTAMP(event)   (QUANT_TICK_EVENT_CAST(event)->exchange_timestamp)

typedef struct _QuantMarketEvent {
    QuantEvent parent;
    gint provider;
    gint instrument;
} QuantMarketEvent;

typedef struct _QuantTickEvent {
    QuantMarketEvent parent;
    datetime_t exchange_timestamp;
    double price;
    glong size;
} QuantTickEvent;

typedef QuantTickEvent QuantAskEvent;
typedef QuantTickEvent QuantBidEvent;
typedef QuantTickEvent QuantTradeEvent;

QUANT_FUNC QuantEvent* quant_event_tick_init(QuantEvent *e, gint provider, gint instrument, datetime_t exchange_timestamp, double price, glong size);

typedef struct _QuantNewsEvent {
    QuantMarketEvent parent;
    gchar *urgency;
    gchar *url;
    gchar *headline;
    gchar *text;
} QuantNewsEvent;

QUANT_FUNC QuantEvent* quant_event_news_init(QuantEvent *e, gint provider, gint instrument, gchar *urgency, gchar *url, gchar *headline, gchar *text);
QUANT_FUNC void quant_event_news_destory(QuantEvent *e);

typedef struct _QuantFundamentalEvent {
    QuantMarketEvent parent;
} QuantFundamentalEvent;

QUANT_FUNC QuantEvent* quant_event_fundamental_init(QuantEvent *e, gint provider, gint instrument);

QUANT_END_DECLS

#endif // QUANT_EVENT_MARKET_H
