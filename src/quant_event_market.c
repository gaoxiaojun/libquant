/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include "quant_event_market.h"

static QuantTickEvent * quant_event_new_tick(gint instrument, gint provider, datetime_t exchange_timestamp, double price, glong size)
{
    QuantTickEvent * tick = g_new0(QuantTickEvent, 1);

    if(tick) {
        tick->exchange_timestamp = exchange_timestamp;
        tick->instrument = instrument;
        tick->provider = provider;
        tick->price = price;
        tick->size = size;
    }

    return tick;
}


QuantTickEvent * quant_event_new_ask(gint instrument, gint provider, datetime_t exchange_timestamp, double price, glong size)
{
    return quant_event_new_tick(instrument, provider, exchange_timestamp, price, size);
}

QuantTickEvent * quant_event_new_bid(gint instrument, gint provider, datetime_t exchange_timestamp, double price, glong size)
{
    return quant_event_new_tick(instrument, provider, exchange_timestamp, price, size);
}

QuantTickEvent * quant_event_new_trade(gint instrument, gint provider, datetime_t exchange_timestamp, double price, glong size)
{
    return quant_event_new_tick(instrument, provider, exchange_timestamp, price, size);
}
