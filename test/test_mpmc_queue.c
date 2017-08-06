#include <stdio.h>
#include <stdlib.h>
#include "../src/quant_mpmc_queue.h"
#include "../src/quant_event.h"

#define TEST_COUNT 10000000

int main()
{

    QuantEvent *event = malloc(sizeof(QuantEvent));
    quant_event_init(event);

    QuantAtomicQueue *queue = quant_atomic_queue_new(1024 * 128);

    for (int i = 0; i < TEST_COUNT; i++) {
        quant_event_ref(event);
        quant_atomic_queue_push(queue, event);
    }

   for (int i = 0; i < TEST_COUNT; i++) {
       QuantEvent* e = quant_atomic_queue_pop(queue);
       quant_event_unref(e);
   } 
    g_assert(event->refcount == 1);
    return 0;
}
