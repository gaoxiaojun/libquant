/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include <string.h>

#include "quant_mpmc_queue.h"


/**
 * SECTION:gstatomicqueue
 * @title: QuantAtomicQueue
 * @short_description: An atomic queue implementation
 *
 * The #QuantAtomicQueue object implements a queue that can be used from multiple
 * threads without performing any blocking operations.
 */

G_DEFINE_BOXED_TYPE (QuantAtomicQueue, quant_atomic_queue,
    (GBoxedCopyFunc) quant_atomic_queue_ref,
    (GBoxedFreeFunc) quant_atomic_queue_unref);

/* By default the queue uses 2 * sizeof(gpointer) * clp2 (max_items) of
 * memory. clp2(x) is the next power of two >= than x.
 *
 * The queue can operate in low memory mode, in which it consumes almost
 * half the memory at the expense of extra overhead in the readers. This
 * is disabled by default because even without LOW_MEM mode, the memory
 * consumption is still lower than a plain GList.
 */
#undef LOW_MEM

typedef struct _QuantAQueueMem QuantAQueueMem;

struct _QuantAQueueMem
{
  gint size;
  gpointer *array;
  volatile gint head;
  volatile gint tail_write;
  volatile gint tail_read;
  QuantAQueueMem *next;
  QuantAQueueMem *free;
};

static guint
clp2 (guint n)
{
  guint res = 1;

  while (res < n)
    res <<= 1;

  return res;
}

static QuantAQueueMem *
new_queue_mem (guint size, gint pos)
{
  QuantAQueueMem *mem;

  mem = g_new (QuantAQueueMem, 1);

  /* we keep the size as a mask for performance */
  mem->size = clp2 (MAX (size, 16)) - 1;
  mem->array = g_new0 (gpointer, mem->size + 1);
  mem->head = pos;
  mem->tail_write = pos;
  mem->tail_read = pos;
  mem->next = NULL;
  mem->free = NULL;

  return mem;
}

static void
free_queue_mem (QuantAQueueMem * mem)
{
  g_free (mem->array);
  g_free (mem);
}

struct _QuantAtomicQueue
{
  volatile gint refcount;
#ifdef LOW_MEM
  gint num_readers;
#endif
  QuantAQueueMem *head_mem;
  QuantAQueueMem *tail_mem;
  QuantAQueueMem *free_list;
};

static void
add_to_free_list (QuantAtomicQueue * queue, QuantAQueueMem * mem)
{
  do {
    mem->free = g_atomic_pointer_get (&queue->free_list);
  } while (!g_atomic_pointer_compare_and_exchange (&queue->free_list,
          mem->free, mem));
}

static void
clear_free_list (QuantAtomicQueue * queue)
{
  QuantAQueueMem *free_list;

  /* take the free list and replace with NULL */
  do {
    free_list = g_atomic_pointer_get (&queue->free_list);
    if (free_list == NULL)
      return;
  } while (!g_atomic_pointer_compare_and_exchange (&queue->free_list, free_list,
          NULL));

  while (free_list) {
    QuantAQueueMem *next = free_list->free;

    free_queue_mem (free_list);

    free_list = next;
  }
}

/**
 * quant_atomic_queue_new:
 * @initial_size: initial queue size
 *
 * Create a new atomic queue instance. @initial_size will be rounded up to the
 * nearest power of 2 and used as the initial size of the queue.
 *
 * Returns: a new #QuantAtomicQueue
 */
QuantAtomicQueue *
quant_atomic_queue_new (guint initial_size)
{
  QuantAtomicQueue *queue;

  queue = g_new (QuantAtomicQueue, 1);

  queue->refcount = 1;
#ifdef LOW_MEM
  queue->num_readers = 0;
#endif
  queue->head_mem = queue->tail_mem = new_queue_mem (initial_size, 0);
  queue->free_list = NULL;

  return queue;
}

/**
 * quant_atomic_queue_ref:
 * @queue: a #QuantAtomicQueue
 *
 * Increase the refcount of @queue.
 */
void
quant_atomic_queue_ref (QuantAtomicQueue * queue)
{
  g_return_if_fail (queue != NULL);

  g_atomic_int_inc (&queue->refcount);
}

static void
quant_atomic_queue_free (QuantAtomicQueue * queue)
{
  free_queue_mem (queue->head_mem);
  if (queue->head_mem != queue->tail_mem)
    free_queue_mem (queue->tail_mem);
  clear_free_list (queue);
  g_free (queue);
}

/**
 * quant_atomic_queue_unref:
 * @queue: a #QuantAtomicQueue
 *
 * Unref @queue and free the memory when the refcount reaches 0.
 */
void
quant_atomic_queue_unref (QuantAtomicQueue * queue)
{
  g_return_if_fail (queue != NULL);

  if (g_atomic_int_dec_and_test (&queue->refcount))
    quant_atomic_queue_free (queue);
}

/**
 * quant_atomic_queue_peek:
 * @queue: a #QuantAtomicQueue
 *
 * Peek the head element of the queue without removing it from the queue.
 *
 * Returns: (transfer none) (nullable): the head element of @queue or
 * %NULL when the queue is empty.
 */
gpointer
quant_atomic_queue_peek (QuantAtomicQueue * queue)
{
  QuantAQueueMem *head_mem;
  gint head, tail, size;

  g_return_val_if_fail (queue != NULL, NULL);

  while (TRUE) {
    QuantAQueueMem *next;

    head_mem = g_atomic_pointer_get (&queue->head_mem);

    head = g_atomic_int_get (&head_mem->head);
    tail = g_atomic_int_get (&head_mem->tail_read);
    size = head_mem->size;

    /* when we are not empty, we can continue */
    if (G_LIKELY (head != tail))
      break;

    /* else array empty, try to take next */
    next = g_atomic_pointer_get (&head_mem->next);
    if (next == NULL)
      return NULL;

    /* now we try to move the next array as the head memory. If we fail to do that,
     * some other reader managed to do it first and we retry */
    if (!g_atomic_pointer_compare_and_exchange (&queue->head_mem, head_mem,
            next))
      continue;

    /* when we managed to swing the head pointer the old head is now
     * useless and we add it to the freelist. We can't free the memory yet
     * because we first need to make sure no reader is accessing it anymore. */
    add_to_free_list (queue, head_mem);
  }

  return head_mem->array[head & size];
}

/**
 * quant_atomic_queue_pop:
 * @queue: a #QuantAtomicQueue
 *
 * Get the head element of the queue.
 *
 * Returns: (transfer full): the head element of @queue or %NULL when
 * the queue is empty.
 */
gpointer
quant_atomic_queue_pop (QuantAtomicQueue * queue)
{
  gpointer ret;
  QuantAQueueMem *head_mem;
  gint head, tail, size;

  g_return_val_if_fail (queue != NULL, NULL);

#ifdef LOW_MEM
  g_atomic_int_inc (&queue->num_readers);
#endif

  do {
    while (TRUE) {
      QuantAQueueMem *next;

      head_mem = g_atomic_pointer_get (&queue->head_mem);

      head = g_atomic_int_get (&head_mem->head);
      tail = g_atomic_int_get (&head_mem->tail_read);
      size = head_mem->size;

      /* when we are not empty, we can continue */
      if G_LIKELY
        (head != tail)
            break;

      /* else array empty, try to take next */
      next = g_atomic_pointer_get (&head_mem->next);
      if (next == NULL)
        return NULL;

      /* now we try to move the next array as the head memory. If we fail to do that,
       * some other reader managed to do it first and we retry */
      if G_UNLIKELY
        (!g_atomic_pointer_compare_and_exchange (&queue->head_mem, head_mem,
                next))
            continue;

      /* when we managed to swing the head pointer the old head is now
       * useless and we add it to the freelist. We can't free the memory yet
       * because we first need to make sure no reader is accessing it anymore. */
      add_to_free_list (queue, head_mem);
    }

    ret = head_mem->array[head & size];
  } while G_UNLIKELY
  (!g_atomic_int_compare_and_exchange (&head_mem->head, head, head + 1));

#ifdef LOW_MEM
  /* decrement number of readers, when we reach 0 readers we can be sure that
   * none is accessing the memory in the free list and we can try to clean up */
  if (g_atomic_int_dec_and_test (&queue->num_readers))
    clear_free_list (queue);
#endif

  return ret;
}

/**
 * quant_atomic_queue_push:
 * @queue: a #QuantAtomicQueue
 * @data: the data
 *
 * Append @data to the tail of the queue.
 */
void
quant_atomic_queue_push (QuantAtomicQueue * queue, gpointer data)
{
  QuantAQueueMem *tail_mem;
  gint head, tail, size;

  g_return_if_fail (queue != NULL);

  do {
    while (TRUE) {
      QuantAQueueMem *mem;

      tail_mem = g_atomic_pointer_get (&queue->tail_mem);
      head = g_atomic_int_get (&tail_mem->head);
      tail = g_atomic_int_get (&tail_mem->tail_write);
      size = tail_mem->size;

      /* we're not full, continue */
      if G_LIKELY
        (tail - head <= size)
            break;

      /* else we need to grow the array, we store a mask so we have to add 1 */
      mem = new_queue_mem ((size << 1) + 1, tail);

      /* try to make our new array visible to other writers */
      if G_UNLIKELY
        (!g_atomic_pointer_compare_and_exchange (&queue->tail_mem, tail_mem,
                mem)) {
        /* we tried to swap the new writer array but something changed. This is
         * because some other writer beat us to it, we free our memory and try
         * again */
        free_queue_mem (mem);
        continue;
        }
      /* make sure that readers can find our new array as well. The one who
       * manages to swap the pointer is the only one who can set the next
       * pointer to the new array */
      g_atomic_pointer_set (&tail_mem->next, mem);
    }
  } while G_UNLIKELY
  (!g_atomic_int_compare_and_exchange (&tail_mem->tail_write, tail, tail + 1));

  tail_mem->array[tail & size] = data;

  /* now wait until all writers have completed their write before we move the
   * tail_read to this new item. It is possible that other writers are still
   * updating the previous array slots and we don't want to reveal their changes
   * before they are done. FIXME, it would be nice if we didn't have to busy
   * wait here. */
  while G_UNLIKELY
    (!g_atomic_int_compare_and_exchange (&tail_mem->tail_read, tail, tail + 1));
}

/**
 * quant_atomic_queue_length:
 * @queue: a #QuantAtomicQueue
 *
 * Get the amount of items in the queue.
 *
 * Returns: the number of elements in the queue.
 */
guint
quant_atomic_queue_length (QuantAtomicQueue * queue)
{
  QuantAQueueMem *head_mem, *tail_mem;
  gint head, tail;

  g_return_val_if_fail (queue != NULL, 0);

#ifdef LOW_MEM
  g_atomic_int_inc (&queue->num_readers);
#endif

  head_mem = g_atomic_pointer_get (&queue->head_mem);
  head = g_atomic_int_get (&head_mem->head);

  tail_mem = g_atomic_pointer_get (&queue->tail_mem);
  tail = g_atomic_int_get (&tail_mem->tail_read);

#ifdef LOW_MEM
  if (g_atomic_int_dec_and_test (&queue->num_readers))
    clear_free_list (queue);
#endif

  return tail - head;
}
