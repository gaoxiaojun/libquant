/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "../src/quant_event_bus.h"
#include "../src/quant_event.h"
#include "../src/quant_event_market.h"

#define TEST_COUNT 10000000

int main()
{

    QuantEventBus *bus = quant_event_bus_new(NULL);
    for (int i = 0 ; i < TEST_COUNT; i++) {
        QuantEvent *event = (QuantEvent*)quant_event_new_ask(0, 1, MIN_DATE_TIME, i, i);
        event->timestamp = datetime_now();

        quant_event_ref(event);
        quant_event_bus_push(bus, event);

        quant_event_unref(event);
    }

    datetime_t time = MIN_DATE_TIME;
   for (int i = 0; i < TEST_COUNT; i++) {
       QuantEvent* e = quant_event_bus_pop(bus);
       g_assert(e->timestamp >= time);
       time = e->timestamp;
       quant_event_unref(e);
   }

    QuantEvent *e = quant_event_bus_pop(bus);
    g_assert(e == NULL);
    return 0;
}
