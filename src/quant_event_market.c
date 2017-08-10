/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include "quant_event_market.h"

QuantEvent * quant_event_tick_init(QuantEvent *e, gint instrument, gint provider, datetime_t exchange_timestamp, double price, glong size)
{
    QuantTickEvent * tick = (QuantTickEvent *)e;
    tick->exchange_timestamp = exchange_timestamp;
    tick->parent.instrument = instrument;
    tick->parent.provider = provider;
    tick->price = price;
    tick->size = size;
    return e;
}


QuantEvent* quant_event_news_init(QuantEvent *e, gint provider, gint instrument, gchar *urgency, gchar *url, gchar *headline, gchar *text)
{
    QuantNewsEvent *news = (QuantNewsEvent *)e;

    news->parent.provider = provider;
    news->parent.instrument = instrument;
    news->urgency = g_strdup(urgency);
    news->headline = g_strdup(headline);
    news->url = g_strdup(url);
    news->text = g_strdup(text);

    return e;
}

#define free_if(x) if(x) free(x);
void quant_event_news_destory(QuantEvent *e)
{
     QuantNewsEvent *news = (QuantNewsEvent *)e;
     free_if(news->urgency);
     free_if(news->headline);
     free_if(news->url);
     free_if(news->text);
}

QuantEvent* quant_event_fundamental_init(QuantEvent *e, gint provider, gint instrument)
{
    QuantFundamentalEvent *fundamental = (QuantFundamentalEvent *)e;
    fundamental->parent.provider = provider;
    fundamental->parent.instrument = instrument;
    return e;
}
