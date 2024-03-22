#pragma once

#include "defines.h"

/*
 * Memory layout
 * u64 capacity = number elements that can be held
 * u64 length = number of elements currently contained
 * u64 stride = size of each element in bytes
 * void* elements
 */

enum {
    DARRAY_CAPACITY,
    DARRAY_LENGTH,
    DARRAY_STRIDE,
    DARRAY_FIELD_LENGTH
};

FAPI void *_darrayCreate(u64 length, u64 stride);
FAPI void _darrayDestroy(void* array);

FAPI u64 _darrayFieldGet(void *array, u64 field);
FAPI void _darrayFieldSet(void *array, u64 field, u64 value);

FAPI void* _darrayResize(void *array);

FAPI void* _darrayPush(void *array, const void *valuePtr);
FAPI void _darrayPop(void *array, void *dest);

FAPI void* _darrayPopAt(void *array, u64 index, void *dest);
FAPI void* _darrayInsertAt(void *array, u64 index, void *valuePtr);

#define DARRAY_DEFAULT_CAPACITY 1
#define DARRAY_RESIZE_FACTOR 2

#define darrayCreate(type)                               \
    _darrayCreate(DARRAY_DEFAULT_CAPACITY, sizeof(type)) \

#define darrayReserve(type, capacity)     \
    _darrayCreate(capacity, sizeof(type)) \

#define darrayDestroy(array) _darrayDestroy(array);

#define darrayPush(array, value) {     \
    typeof(value) temp = value;        \
    array = _darrayPush(array, &temp); \
}                                      \

/* NOTE: could use __auto_type for temp above, but intellisense
 * for VSCode flags it as an unknown type. typeof() seems to
 * work just fine, though. Both are GNU extensions.
 */

#define darrayPop(array, value_ptr) \
    _darrayPop(array, value_ptr)     \

#define darrayInsertAt(array, index, value) {     \
    typeof(value) temp = value;                   \
    array = _darrayInsertAt(array, index, &temp); \
}                                                 \

#define darrayPopAt(array, index, valuePtr) \
    _darrayPopAt(array, index, valuePtr)    \

#define darrayClear(array)                   \
    _darrayFieldSet(array, DARRAY_LENGTH, 0) \

#define darrayCapacity(array)               \
    _darrayFieldGet(array, DARRAY_CAPACITY) \

#define darrayLength(array)               \
    _darrayFieldGet(array, DARRAY_LENGTH) \

#define darrayStride(array)               \
    _darrayFieldGet(array, DARRAY_STRIDE) \

#define darrayLengthSet(array, value)            \
    _darrayFieldSet(array, DARRAY_LENGTH, value) \
