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

#define QUANT_TICK_EVENT_CAST(obj)              ((QuantTickEvent *)(obj))

#define QUANT_EVENT_EXCHANGE_TIMESTAMP(event)    (QUANT_TICK_EVENT_CAST(event)->exchange_timestamp)

typedef struct _QuantTickEvent {
    QuantEvent parent;
    datetime_t exchange_timestamp;
    gint instrument;
    gint provider;
    double price;
    glong size;
}QuantTickEvent;


typedef QuantTickEvent QuantAskEvent;
typedef QuantTickEvent QuantBidEvent;
typedef QuantTickEvent QuantTradeEvent;

QUANT_EXPORT QuantTickEvent * quant_event_new_ask(gint instrument, gint provider, datetime_t exchange_timestamp, double price, glong size) G_GNUC_MALLOC;

QUANT_EXPORT QuantTickEvent * quant_event_new_bid(gint instrument, gint provider, datetime_t exchange_timestamp, double price, glong size) G_GNUC_MALLOC;

QUANT_EXPORT QuantTickEvent * quant_event_new_trade(gint instrument, gint provider, datetime_t exchange_timestamp, double price, glong size) G_GNUC_MALLOC;

G_END_DECLS

#endif // QUANT_EVENT_MARKET_H
