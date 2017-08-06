#include <stdio.h>
#include <stdlib.h>
#include "../src/quant_event_queue.h"
#include "../src/quant_event.h"
#include <assert.h>

#define TEST_COUNT 10000000

int main()
{

    QuantEvent *event = malloc(sizeof(QuantEvent));
    quant_event_init(event);

    QuantEventQueue *queue = quant_event_queue_new();

    for (int i = 0; i < TEST_COUNT; i++) {
        quant_event_ref(event);
        quant_event_queue_push(queue, event);
    }

   for (int i = 0; i < TEST_COUNT; i++) {
       QuantEvent* e = quant_event_queue_pop(queue);
       quant_event_unref(e);
   }
    assert(event->refcount == 1);
    return 0;
}
