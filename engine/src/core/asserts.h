#pragma once

#include "defines.h"

/* Disable assertions by commenting out the below line. */
#define FASSERTIONS_ENABLED

#ifdef FASSERTIONS_ENABLED
#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

FAPI void reportAssertionFailure(const char *expression, const char *message, const char *file, i32 line);

#define FASSERT(expression) {                                        \
    if (expression) {} else {                                        \
        reportAssertionFailure(#expression, "", __FILE__, __LINE__); \
        debugBreak();                                                \
    }                                                                \
}                                                                    \

#define FASSERT_MSG(expression, message) {                                \
    if (expression) {} else {                                             \
        reportAssertionFailure(#expression, message, __FILE__, __LINE__); \
        debugBreak();                                                     \
    }                                                                     \
}                                                                         \

#ifdef _DEBUG
#define FASSERT_DEBUG(expr) {                                  \
    if (expr) {} else {                                        \
        reportAssertionFailure(#expr, "", __FILE__, __LINE__); \
        debugBreak();                                          \
    }                                                          \
}                                                              \

#else
#define FASSERT_DEBUG(expression)
#endif

#else
#define FASSERT(expression)
#define FASSERT_MSG(expression, message)
#define FASSERT_DEBUG(expression)
#endif