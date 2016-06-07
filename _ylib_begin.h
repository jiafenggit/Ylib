#ifndef __YLIB_H__
#define __YLIB_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

#include <stdio.h>
// for malloc() and free()
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
// for boolean type (bool) and values (true, false)
#include <stdbool.h>
// for integer types (int8_t, uint8_t, int16_t, uint16_t, int32_t uint32_t, int64_t, uint64_t)
#include <stdint.h>

#ifdef USE_BOEHM_GC
# include "gc.h"
#endif /* USE_BOEHM_GC */

