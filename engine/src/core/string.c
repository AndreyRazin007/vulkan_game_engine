#include "core/string.h"
#include "core/memory.h"

#include <string.h>

u64 stringLength(const char *str) {
    return strlen(str);
}

char *stringDuplicate(const char *str) {
    u64 length = stringLength(str);
    char *copy = allocate(length + 1, MEMORY_TAG_STRING);
    copyMemory(copy, str, length + 1);

    return copy;
}
