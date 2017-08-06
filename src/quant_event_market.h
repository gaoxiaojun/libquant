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

G_BEGIN_DECLS

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

QUANT_EXPORT QuantTickEvent* quant_event_new_ask(gint provider, gint instrument, datetime_t exchange_timestamp, double price, glong size) G_GNUC_MALLOC;

QUANT_EXPORT QuantTickEvent* quant_event_new_bid(gint provider, gint instrument, datetime_t exchange_timestamp, double price, glong size) G_GNUC_MALLOC;

QUANT_EXPORT QuantTickEvent* quant_event_new_trade(gint provider, gint instrument, datetime_t exchange_timestamp, double price, glong size) G_GNUC_MALLOC;

typedef struct _QuantNewsEvent {
    QuantMarketEvent parent;
    gchar *urgency;
    gchar *url;
    gchar *headline;
    gchar *text;
} QuantNewsEvent;

QUANT_EXPORT QuantNewsEvent* quant_event_new_news(gint provider, gint instrument, gchar *urgency, gchar *url, gchar *headline, gchar *text) G_GNUC_MALLOC;

typedef struct _QuantFundamentalEvent {
    QuantMarketEvent parent;
} QuantFundamentalEvent;

QUANT_EXPORT QuantFundamentalEvent* quant_event_new_fundamental(gint provider, gint instrument) G_GNUC_MALLOC;

G_END_DECLS

#endif // QUANT_EVENT_MARKET_H
