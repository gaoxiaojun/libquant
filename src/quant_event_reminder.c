/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#include "quant_event_reminder.h"

QuantReminderEvent *quant_event_new_reminder(QuantReminderEventClockType ctype, reminder_cb callback, void *data)
{
    QuantReminderEvent *revent = g_new0(QuantReminderEvent, 1);

    if (revent) {
        revent->clock_type = ctype;
        revent->callback = callback;
        revent->user_data = data;
    }

    return revent;
}
