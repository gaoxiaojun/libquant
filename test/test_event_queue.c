#include <stdio.h>
#include <stdlib.h>
#include "../src/quant_event_queue.h"
#include "../src/quant_event.h"

#define TEST_COUNT 10000000

int main()
{

    QuantEvent *event = quant_event_new(QUANT_EVENT_ASK);

    QuantEventQueue *queue = quant_event_queue_new();

    for (int i = 0; i < TEST_COUNT; i++) {
        quant_event_ref(event);
        quant_event_queue_push(queue, event);
    }

   for (int i = 0; i < TEST_COUNT; i++) {
       QuantEvent* e = quant_event_queue_pop(queue);
       quant_event_unref(e);
   }
    g_assert(event->refcount == 1);
    return 0;
}
