/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include "quant_event_queue.h"
#include "adt_tree.h"
#include <assert.h>
#include <jemalloc/jemalloc.h>

RB_HEAD(event_tree_s, event_node_s);

struct _QuantEventQueue {
    volatile gint refcount;
    struct event_tree_s tree;
    guint64 counter;
};

typedef struct event_node_s {
    RB_ENTRY(event_node_s)
    rbentry;
    QuantEvent* event;
    guint64 id;
} event_node_t;

int event_node_compare(event_node_t* lhs, event_node_t* rhs)
{
    guint64 lhs_time = lhs->event->timestamp;
    guint64 rhs_time = rhs->event->timestamp;

    if (lhs_time < rhs_time)
        return -1;
    if (lhs_time > rhs_time)
        return 1;

    if (lhs->id < rhs->id)
        return -1;
    if (lhs->id > rhs->id)
        return 1;

    return 0;
}

RB_GENERATE_STATIC(event_tree_s, event_node_s, rbentry, event_node_compare)

/* for custom node alloc and free */
static inline event_node_t* node_new() {
        return calloc(1, sizeof(event_node_t));
}

static inline void node_free(event_node_t* node) {
    free(node);
}

/*  convenient red-black tree operator */
static inline event_node_t* rbtree_min(QuantEventQueue* q)
{
    return RB_MIN(event_tree_s, &q->tree);
}

static inline event_node_t* rbtree_remove(QuantEventQueue* q,
    event_node_t* node)
{
    return RB_REMOVE(event_tree_s, &q->tree, node);
}

static inline event_node_t* rbtree_insert(QuantEventQueue* q,
    event_node_t* node)
{
    return RB_INSERT(event_tree_s, &q->tree, node);
}

static inline event_node_t* rbtree_find(QuantEventQueue* q, QuantEvent* e)
{
    event_node_t node;
    node.event = e;
    return RB_FIND(event_tree_s, &q->tree, &node);
}

/* API */
QuantEventQueue * quant_event_queue_new()
{
    QuantEventQueue *queue;

    queue = g_new (QuantEventQueue, 1);

    queue->refcount = 1;

    RB_INIT(&queue->tree);
    queue->counter = 1;

    return queue;
}

void quant_event_queue_ref (QuantEventQueue *queue)
{
    g_return_if_fail( queue != NULL);
    g_atomic_int_inc(&queue->refcount);
}

static void
quant_event_queue_free (QuantEventQueue * queue)
{
    quant_event_queue_clear(queue);
    g_free (queue);
}

void quant_event_queue_unref (QuantEventQueue *queue)
{
    g_return_if_fail (queue != NULL);

    if (g_atomic_int_dec_and_test (&queue->refcount))
      quant_event_queue_free (queue);
}

/* add reminder
 * if e has been insert return EDUP
 * if malloc failed return ENOMEM
 * other return 0
 */
guint64 quant_event_queue_push(QuantEventQueue* q, QuantEvent* e)
{
    g_return_val_if_fail( e != NULL, EPARAM);
    g_return_val_if_fail (q != NULL, EPARAM);

    event_node_t* node = node_new();
    if (G_UNLIKELY(!node))
        return ENOMEM;
    node->event = e;
    node->id = q->counter++;
    event_node_t* found = rbtree_insert(q, node);
    if (G_UNLIKELY(found)) {
        assert(found->event == e);
        node_free(node);
        return EDUPLICATE;
    }

    return node->id;
}

QuantEvent* quant_event_queue_pop(QuantEventQueue* q)
{
    QuantEvent *e;
    event_node_t* node = rbtree_min(q);

    if (G_LIKELY(node)) {
        e = node->event;
        rbtree_remove(q, node);
        node_free(node);
        return e;
    } else {
        return NULL;
    }
}

QuantEvent* quant_event_queue_peek(QuantEventQueue* q)
{
    event_node_t* node = rbtree_min(q);

    return node != NULL ? node->event : NULL;
}

int quant_event_queue_is_empty(QuantEventQueue* q) { return RB_EMPTY(&q->tree); }

void quant_event_queue_clear(QuantEventQueue* q)
{
    event_node_t *np, *nn;
    RB_FOREACH_SAFE(np, event_tree_s, &q->tree, nn)
    {
        rbtree_remove(q, np);
        node_free(np);
    }
}

int quant_event_queue_remove(QuantEventQueue* q, QuantEvent *e)
{
    event_node_t fnode;
    fnode.event = e;
    fnode.id = 0; // UNUSED
    event_node_t* node = RB_FIND(event_tree_s, &q->tree, &fnode);
    if (!node)
        return -1; // not found

    rbtree_remove(q, node);
    node_free(node);
    return 0;
}

int quant_event_queue_remove_by_id(QuantEventQueue* q, guint64 id)
{
    event_node_t fnode;
    fnode.event = NULL; // UNUSED
    fnode.id = id;
    event_node_t* node = RB_FIND(event_tree_s, &q->tree, &fnode);
    if (!node)
        return -1; // not found

    rbtree_remove(q, node);
    node_free(node);
    return 0;
}
