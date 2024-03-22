#include "containers/darray.h"

#include "core/memory.h"
#include "core/logger.h"

void* _darrayCreate(u64 length, u64 stride) {
    u64 headerSize = DARRAY_FIELD_LENGTH * sizeof(u64);
    u64 arraySize = length * stride;
    u64* newArray = allocate(headerSize + arraySize, MEMORY_TAG_DARRAY);
    setMemory(newArray, 0, headerSize + arraySize);
    newArray[DARRAY_CAPACITY] = length;
    newArray[DARRAY_LENGTH] = 0;
    newArray[DARRAY_STRIDE] = stride;
    return (void*)(newArray + DARRAY_FIELD_LENGTH);
}

void _darrayDestroy(void *array) {
    u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
    u64 headerSize = DARRAY_FIELD_LENGTH * sizeof(u64);
    u64 totalSize = headerSize + header[DARRAY_CAPACITY] * header[DARRAY_STRIDE];
    free(header, totalSize, MEMORY_TAG_DARRAY);
}

u64 _darrayFieldGet(void *array, u64 field) {
    u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
    return header[field];
}

void _darrayFieldSet(void *array, u64 field, u64 value) {
    u64* header = (u64*)array - DARRAY_FIELD_LENGTH;
    header[field] = value;
}

void *_darrayResize(void *array) {
    u64 length = darrayLength(array);
    u64 stride = darrayStride(array);
    void* temp = _darrayCreate(
        (DARRAY_RESIZE_FACTOR * darrayCapacity(array)),
        stride);
    copyMemory(temp, array, length * stride);

    _darrayFieldSet(temp, DARRAY_LENGTH, length);
    _darrayDestroy(array);

    return temp;
}

void *_darrayPush(void *array, const void *valuePtr) {
    u64 length = darrayLength(array);
    u64 stride = darrayStride(array);
    if (length >= darrayCapacity(array)) {
        array = _darrayResize(array);
    }

    u64 address = (u64)array;
    address += (length * stride);
    copyMemory((void*)address, valuePtr, stride);
    _darrayFieldSet(array, DARRAY_LENGTH, length + 1);

    return array;
}

void _darrayPop(void *array, void *dest) {
    u64 length = darrayLength(array);
    u64 stride = darrayStride(array);
    u64 address = (u64)array;
    address += ((length - 1) * stride);
    copyMemory(dest, (void*)address, stride);
    _darrayFieldSet(array, DARRAY_LENGTH, length - 1);
}

void *_darrayPopAt(void *array, u64 index, void *dest) {
    u64 length = darrayLength(array);
    u64 stride = darrayStride(array);
    if (index >= length) {
        FERROR("Index outside the bounds of this array! Length: %i, index: %index",
               length, index);

        return array;
    }

    u64 address = (u64)array;
    copyMemory(dest, (void*)(address + (index * stride)), stride);

    /* If not on the last element, snip out the entry and copy the rest inward. */
    if (index != length - 1) {
        copyMemory(
            (void*)(address + (index * stride)),
            (void*)(address + ((index + 1) * stride)),
            stride * (length - index));
    }

    _darrayFieldSet(array, DARRAY_LENGTH, length - 1);

    return array;
}

void* _darrayInsertAt(void *array, u64 index, void *valuePtr) {
    u64 length = darrayLength(array);
    u64 stride = darrayStride(array);
    if (index >= length) {
        FERROR("Index outside the bounds of this array! Length: %i, index: %index", length, index);

        return array;
    }

    if (length >= darrayCapacity(array)) {
        array = _darrayResize(array);
    }

    u64 address = (u64)array;

    /* If not on the last element, copy the rest outward. */
    if (index != length - 1) {
        copyMemory((void*)(address + ((index + 1) * stride)),
                   (void*)(address + (index * stride)),
                   stride * (length - index));
    }

    /* Set the value at the index */
    copyMemory((void*)(address + (index * stride)), valuePtr, stride);

    _darrayFieldSet(array, DARRAY_LENGTH, length + 1);

    return array;
}
