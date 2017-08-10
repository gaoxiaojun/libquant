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

/* Various macros. */
#ifndef UNUSED
#define UNUSED(x)	((void)(x))	/* to avoid warnings */
#endif

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

#ifdef  __cplusplus
# define QUANT_BEGIN_DECLS  extern "C" {
# define QUANT_END_DECLS    }
#else
# define QUANT_BEGIN_DECLS
# define QUANT_END_DECLS
#endif


#if defined(__GNUC__)
#define QUANT_NORET     __attribute__((noreturn))
#define QUANT_ALIGN(n)	__attribute__((aligned(n)))
#define QUANT_INLINE	inline
#define QUANT_AINLINE	inline __attribute__((always_inline))
#define QUANT_NOINLINE	__attribute__((noinline))

#if defined(__ELF__) || defined(__MACH__)
#define QUANT_NOAPI	extern __attribute__((visibility("hidden")))
#endif

#define QUANT_LIKELY(x)     __builtin_expect(!!(x), 1)
#define QUANT_UNLIKELY(x)	__builtin_expect(!!(x), 0)

#elif defined(_MSC_VER)

#define QUANT_NORET     __declspec(noreturn)
#define QUANT_ALIGN(n)	__declspec(align(n))
#define QUANT_INLINE	__inline
#define QUANT_AINLINE	__forceinline
#define QUANT_NOINLINE	__declspec(noinline)

#else
#error "missing defines for your compiler"
#endif

/* Attributes for internal functions. */
#define QUANT_DATA		QUANT_NOAPI
#define QUANT_DATADEF
#define QUANT_ASMF		QUANT_NOAPI
#define QUANT_FUNCA     QUANT_NOAPI
#if defined(QUANT_AMALG_C)
#define QUANT_FUNC		static
#else
#define QUANT_FUNC		QUANT_NOAPI
#endif
#define QUANT_FUNC_NORET	QUANT_FUNC  QUANT_NORET
#define QUANT_FUNCA_NORET	QUANT_FUNCA QUANT_NORET
#define QUANT_ASMF_NORET	QUANT_ASMF  QUANT_NORET

/* convien macros */


/* common include files */
#if defined(_MSC_VER) && _MSC_VER < 1600
#include "stdint-msvc2008.h"
#else
#include <stdint.h>
#endif

/* Needed everywhere. */
#include <string.h>
#include <stdlib.h>

#endif /* __QUANT_MACROS_H__ */
