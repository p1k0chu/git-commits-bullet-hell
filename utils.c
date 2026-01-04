
#include <stddef.h>

const char *strnchr(const char *s, int c, size_t n) {
    const char *ptr;
    for (ptr = s; ptr < s + n; ptr++) {
        if (*ptr == c) return ptr;
    }
    return (void *)0;
}

