/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include <glib.h>
#include <glib-object.h>
#include "quant_config.h"


#ifndef __QUANT_ATOMIC_QUEUE_H__
#define __QUANT_ATOMIC_QUEUE_H__

QUANT_BEGIN_DECLS

#define QUANT_TYPE_ATOMIC_QUEUE (quant_atomic_queue_get_type())

/**
 * QuantAtomicQueue:
 *
 * Opaque atomic data queue.
 *
 * Use the accessor functions to get the stored values.
 */
typedef struct _QuantAtomicQueue QuantAtomicQueue;


QUANT_EXPORT
GType              quant_atomic_queue_get_type    (void);

QUANT_EXPORT
QuantAtomicQueue *   quant_atomic_queue_new         (guint initial_size) G_GNUC_MALLOC;

QUANT_EXPORT
void               quant_atomic_queue_ref         (QuantAtomicQueue * queue);

QUANT_EXPORT
void               quant_atomic_queue_unref       (QuantAtomicQueue * queue);

QUANT_EXPORT
void               quant_atomic_queue_push        (QuantAtomicQueue* queue, gpointer data);

QUANT_EXPORT
gpointer           quant_atomic_queue_pop         (QuantAtomicQueue* queue);

QUANT_EXPORT
gpointer           quant_atomic_queue_peek        (QuantAtomicQueue* queue);

QUANT_EXPORT
guint              quant_atomic_queue_length      (QuantAtomicQueue * queue);

#ifdef G_DEFINE_AUTOPTR_CLEANUP_FUNC
G_DEFINE_AUTOPTR_CLEANUP_FUNC(QuantAtomicQueue, quant_atomic_queue_unref)
#endif

QUANT_END_DECLS

#endif /* __QUANT_ATOMIC_QUEUE_H__ */
