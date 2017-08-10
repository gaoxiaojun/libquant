/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#ifndef QUANT_EVENT_REMINDER_H
#define QUANT_EVENT_REMINDER_H

#include "quant_event.h"

QUANT_BEGIN_DECLS

typedef void (*reminder_cb)(datetime_t time, void *data);

typedef enum {
    QUANT_REMINDER_EVENT_LOCAL_CLOCK,
    QUANT_REMINDER_EVENT_EXCHANGE_CLOCK
}QuantReminderEventClockType;

typedef struct _QuantReminderEvent {
    QuantEvent parent;
    QuantReminderEventClockType clock_type;
    reminder_cb callback;
    void *user_data;
}QuantReminderEvent;

QUANT_FUNC QuantEvent *quant_event_reminder_init(QuantEvent *e, QuantReminderEventClockType ctype, reminder_cb callback, void *data);


QUANT_END_DECLS

#endif // QUANT_EVENT_REMINDER_H
