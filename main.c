#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define die(s) fprintf(stderr, "%s\n", s);

int main() {
    int   git_pipe[2];
    pid_t pid;
    char  buffer[32];

    if (pipe(git_pipe) == -1) die("pipe");
    if ((pid = fork()) == -1) die("fork");

    if (pid == 0) {
        dup2(git_pipe[1], STDOUT_FILENO);
        close(git_pipe[0]);
        close(git_pipe[1]);
        execlp("git", "git", "log", "--pretty=%s", NULL);
        exit(0);
    }

    close(git_pipe[1]);

    int nbytes;
    while ((nbytes = read(git_pipe[0], buffer, sizeof(buffer))) > 0) {
        printf("%.*s", nbytes, buffer);
    }
    close(git_pipe[0]);
    if (nbytes == -1) die("read");
}

