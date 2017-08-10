/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include "../src/quant_event.h"
#include "../src/quant_event_bus.h"
#include "../src/quant_event_market.h"
#include <stdio.h>
#include <stdlib.h>

#define TEST_COUNT 10240000
#define TEST_ITER  102400
int main()
{
    datetime_t time = MIN_DATE_TIME;
    QuantEventBus* bus = quant_event_bus_new(NULL);
    for (int j = 0; j < TEST_COUNT / TEST_ITER; j++) {
        for (int i = 0; i < TEST_ITER; i++) {
            QuantEvent* event = (QuantEvent*)quant_event_new(QUANT_EVENT_ASK, 0, 1, MIN_DATE_TIME, i, i);
            event->timestamp = datetime_now();

            quant_event_ref(event);
            quant_event_bus_push(bus, event);

            quant_event_unref(event);
        }

        for (int i = 0; i < TEST_ITER; i++) {
            QuantEvent* e = quant_event_bus_pop(bus);
            if (e) {
                g_assert(e->timestamp >= time);
                time = e->timestamp;
                quant_event_unref(e);
            }
        }
    }
    QuantEvent* e = quant_event_bus_pop(bus);
    g_assert(e == NULL);

    return 0;
}
