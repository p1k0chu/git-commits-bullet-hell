
#include "git2/errors.h"
#include "stdio.h"

#include <stddef.h>

const char *strnchr(const char *s, int c, size_t n) {
    const char *ptr;
    for (ptr = s; ptr < s + n; ptr++) {
        if (*ptr == c)
            return ptr;
    }
    return (void *)0;
}

void libgit_panic(int error) {
    const git_error *s = git_error_last();
    fprintf(stderr, "libgit2 error: %d/%d: %s\n", error, s->klass, s->message);
    exit(1);
}

