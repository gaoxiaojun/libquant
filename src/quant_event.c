/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include "quant_event.h"

void
quant_event_free(QuantEvent *event)
{
    g_free(event);
}

void quant_event_init(QuantEvent *event)
{
    quant_event_ref(event);
}

QuantEvent *quant_event_copy(const QuantEvent *event)
{
    return NULL;
}

QuantEvent *quant_event_new(QuantEventType type)
{
    return NULL;
}
