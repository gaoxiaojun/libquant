/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include "quant_config.h"
#include "adt_tree.h"
#include "quant_event.h"

QUANT_BEGIN_DECLS

/**
 * QuantEventQueue:
 * opaque event queue
 * NOT THREAD SAFE!
 */
typedef struct _QuantEventQueue QuantEventQueue;

QUANT_EXPORT
QuantEventQueue * quant_event_queue_new(void) G_GNUC_MALLOC;

QUANT_EXPORT
void quant_event_queue_ref (QuantEventQueue *queue);

QUANT_EXPORT
void quant_event_queue_unref (QuantEventQueue *queue);


#define ENOMEM      -1
#define EDUPLICATE  -2
#define EPARAM      -3
/* @param QuantEvent IN
 * @result < 0 mean error -1 ENOMEM -2 EDUPLICATE -3 EPARAM
 * other is id
 */
QUANT_EXPORT int quant_event_queue_push(QuantEventQueue* q, QuantEvent* e);

/*
 * return NULL means no event in queue
 */
QUANT_EXPORT QuantEvent* quant_event_queue_pop(QuantEventQueue* q);

QUANT_EXPORT QuantEvent* quant_event_queue_peek(QuantEventQueue* q);

QUANT_EXPORT int quant_event_queue_is_empty(QuantEventQueue* q);

QUANT_EXPORT void quant_event_queue_clear(QuantEventQueue* q);

QUANT_EXPORT int quant_event_queue_remove(QuantEventQueue* q, QuantEvent *e);

QUANT_EXPORT int quant_event_queue_remove_by_id(QuantEventQueue* q, guint64 id);

QUANT_END_DECLS

#endif // __EVENT_QUEUE_H__
