#include "buffer.h"
#include "version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define die(s) fprintf(stderr, "%s\n", s);

int main() {
    if (isatty(STDERR_FILENO)) {
        fprintf(stderr, "Version: %s\n", GitCommitsBulletHell_VERSION);
    }

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

