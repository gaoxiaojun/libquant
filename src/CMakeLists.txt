cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(core C)

set(QUANT_CORE_HEADER
        quant_datetime.h
        quant_event.h
        quant_error.h
        quant_mpmc_queue.h
        quant_event_queue.h
        #quant_loop.h
        #quant_queue.h
        #event_pipe.h
        #reminder_queue.h
        #spsc_queue.h
)
set(QUANT_CORE_SRC
        quant_datetime.c
        quant_event.c
        quant_error.c
        quant_mpmc_queue.c
        quant_event_queue.c
        #quant_loop.c
        #quant_queue.c
        #quant_version.c
        #event_pipe.c
        #reminder_queue.c
        #spsc_queue.c
)

add_library(quant STATIC  ${QUANT_CORE_HEADER} ${QUANT_CORE_SRC})
target_link_libraries(quant ${LIBUV_LIBRARY} ${GLib_LIBRARY} ${GObject_LIBRARY})
