#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define die(s) fprintf(stderr, "%s\n", s);

typedef struct Buffer {
    char *buffer_start;
    int   fd;

    size_t buffer_size;
} Buffer;

const char *strnchr(const char *s, int c, size_t n) {
    for (const char *ptr = s; ptr < s + n; ptr++) {
        if (*ptr == c) return ptr;
    }
    return NULL;
}

char *Buffer_get_line(Buffer *buffer) {
    // remove old line
    size_t len = strnlen(buffer->buffer_start, buffer->buffer_size);
    if (len != buffer->buffer_size) {
        memmove(buffer->buffer_start,
                buffer->buffer_start + len + 1,
                buffer->buffer_size - len - 1);
        memset(buffer->buffer_start + buffer->buffer_size - len - 1, 0, len + 1);
        len = buffer->buffer_size - len - 1;
    }

    char  *end_of_line = NULL;
    ssize_t nbytes;
    len = strnlen(buffer->buffer_start, buffer->buffer_size);
    do {
        if (len >= buffer->buffer_size) {
            char *ptr = realloc(buffer->buffer_start, len + 10);
            if (!ptr) die("realloc");

            buffer->buffer_start = ptr;
            buffer->buffer_size = len + 10;
        }

        nbytes = read(buffer->fd, buffer->buffer_start + len, buffer->buffer_size - len);
        if (nbytes == -1) die("read");

        len += nbytes;
    } while (nbytes != 0 &&
             (end_of_line = (char *)strnchr(buffer->buffer_start, '\n', buffer->buffer_size)) ==
                 NULL);

    if (end_of_line == NULL) {
        if (len >= buffer->buffer_size) {
            char *ptr = realloc(buffer->buffer_start, len + 1);
            if (!ptr) die("realloc");

            buffer->buffer_start = ptr;
            buffer->buffer_size = len + 1;
        }
        buffer->buffer_start[len] = '\0';
    } else {
        *end_of_line = '\0';
    }
    return buffer->buffer_start;
}

int main() {
    int   git_pipe[2];
    pid_t pid;

    Buffer buffer = {.buffer_start = calloc(10, 1), .buffer_size = 10};

    if (pipe(git_pipe) == -1) die("pipe");

    buffer.fd = git_pipe[0];

    if ((pid = fork()) == -1) die("fork");

    if (pid == 0) {
        dup2(git_pipe[1], STDOUT_FILENO);
        close(git_pipe[0]);
        close(git_pipe[1]);
        execlp("git", "git", "log", "--pretty=%s", NULL);
        exit(0);
    }

    close(git_pipe[1]);

    char *ptr;
    while ((ptr = Buffer_get_line(&buffer)) != NULL && strlen(ptr) != 0) {
        printf("commit: \"%s\"\n", ptr);
    }

    close(git_pipe[0]);
    free(buffer.buffer_start);
}

