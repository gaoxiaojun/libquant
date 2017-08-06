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
        tick->parent.instrument = instrument;
        tick->parent.provider = provider;
        tick->price = price;
        tick->size = size;
        QUANT_EVENT_REF_INIT(tick);
    }

    return tick;
}


QuantTickEvent * quant_event_new_ask(gint provider, gint instrument, datetime_t exchange_timestamp, double price, glong size)
{
    return quant_event_new_tick(instrument, provider, exchange_timestamp, price, size);
}

QuantTickEvent * quant_event_new_bid(gint provider, gint instrument, datetime_t exchange_timestamp, double price, glong size)
{
    return quant_event_new_tick(instrument, provider, exchange_timestamp, price, size);
}

QuantTickEvent * quant_event_new_trade(gint provider, gint instrument, datetime_t exchange_timestamp, double price, glong size)
{
    return quant_event_new_tick(instrument, provider, exchange_timestamp, price, size);
}

QuantNewsEvent* quant_event_new_news(gint provider, gint instrument, gchar *urgency, gchar *url, gchar *headline, gchar *text)
{
    QuantNewsEvent *news = g_new0(QuantNewsEvent, 1);
    if (news) {
        news->parent.provider = provider;
        news->parent.instrument = instrument;
        news->urgency = g_strdup(urgency);
        news->headline = g_strdup(headline);
        news->url = g_strdup(url);
        news->text = g_strdup(text);
        QUANT_EVENT_REF_INIT(news);
    }
    return news;
}

QuantFundamentalEvent* quant_event_new_fundamental(gint provider, gint instrument)
{
    QuantFundamentalEvent *fundamental = g_new0(QuantFundamentalEvent, 1);
    if (fundamental) {
        fundamental->parent.provider = provider;
        fundamental->parent.instrument = instrument;
        QUANT_EVENT_REF_INIT(fundamental);
    }
    return fundamental;
}
