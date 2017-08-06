cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(core C)

set(QUANT_CORE_HEADER
        quant_datetime.h
        quant_event.h
        quant_event_reminder.h
        quant_event_market.h
        quant_error.h
        quant_mpmc_queue.h
        quant_event_queue.h
        quant_event_bus.h
)
set(QUANT_CORE_SRC
        quant_datetime.c
        quant_datetime_os.c
        quant_event.c
        quant_event_reminder.c
        quant_event_market.c
        quant_error.c
        quant_mpmc_queue.c
        quant_event_queue.c
        quant_event_bus.c
)

add_library(quant SHARED  ${QUANT_CORE_HEADER} ${QUANT_CORE_SRC})
target_link_libraries(quant ${LIBUV_LIBRARY} ${GLib_LIBRARY} ${GObject_LIBRARY})
