#include <stdio.h>
#include <stdlib.h>
#include "../src/quant_event_queue.h"
#include "../src/quant_event.h"

#define TEST_COUNT 10000000
#define TEST_ITER   10240
int main()
{
    datetime_t time = MIN_DATE_TIME;
    datetime_t create_time = MIN_DATE_TIME;
    QuantEventQueue * queue = quant_event_queue_new();
    for (int j = 0; j < TEST_COUNT / TEST_ITER ; j++) {
        for (int i = 0; i < TEST_ITER; i++) {
            QuantEvent* event = (QuantEvent*)quant_event_new(QUANT_EVENT_ASK,0, 1, MIN_DATE_TIME, i, i);
            event->timestamp = datetime_now();
            g_assert(event->timestamp >= create_time);
            create_time = event->timestamp;

            quant_event_ref(event);
            quant_event_queue_push(queue, event);

            quant_event_unref(event);
        }


        for (int i = 0; i < TEST_ITER; i++) {
            QuantEvent* e = quant_event_queue_pop(queue);
            if (e) {
                if (e->timestamp < time)
            printf("etime = %"G_GUINT64_FORMAT" time=%"G_GUINT64_FORMAT"\n", e->timestamp, time);
            g_assert(e->timestamp >= time);
            time = e->timestamp;
            quant_event_unref(e);
            }
        }
    }
    QuantEvent* e = quant_event_queue_pop(queue);
    g_assert(e == NULL);

    return 0;
}
