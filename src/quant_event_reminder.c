/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include "quant_event_reminder.h"

QuantEvent *quant_event_reminder_init(QuantEvent *e, QuantReminderEventClockType ctype, reminder_cb callback, void *data)
{
    QuantReminderEvent *revent = (QuantReminderEvent *)e;

    revent->clock_type = ctype;
    revent->callback = callback;
    revent->user_data = data;

    return e;
}
