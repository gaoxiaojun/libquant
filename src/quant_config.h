/******************************************************************************
 * Quantitative Kit Library                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License.    *
 ******************************************************************************/
#ifndef __QUANT_MACROS_H__
#define __QUANT_MACROS_H__

#include <glib.h>

/* Static assertions. */
#define QUANT_ASSERT_NAME2(name, line)	name ## line
#define QUANT_ASSERT_NAME(line)		QUANT_ASSERT_NAME2(lj_assert_, line)
#ifdef __COUNTER__
#define QUANT_STATIC_ASSERT(cond) \
  extern void QUANT_ASSERT_NAME(__COUNTER__)(int STATIC_ASSERTION_FAILED[(cond)?1:-1])
#else
#define QUANT_STATIC_ASSERT(cond) \
  extern void QUANT_ASSERT_NAME(__LINE__)(int STATIC_ASSERTION_FAILED[(cond)?1:-1])
#endif


#if (defined(_MSC_VER)) && !defined(QUANT_STATIC_COMPILATION)
# define QUANT_PLUGIN_EXPORT __declspec(dllexport)
# ifdef QUANT_EXPORTS
#  define QUANT_EXPORT __declspec(dllexport)
# else
#  define QUANT_EXPORT __declspec(dllimport) extern
# endif
#else
# if defined(__GNUC__) || (defined(__SUNPRO_C) && (__SUNPRO_C >= 0x590))
#  define QUANT_PLUGIN_EXPORT __attribute__ ((visibility ("default")))
#  define QUANT_EXPORT extern __attribute__ ((visibility ("default")))
# else
#  define QUANT_PLUGIN_EXPORT
#  define QUANT_EXPORT extern
# endif
#endif

#endif /* __QUANT_MACROS_H__ */
