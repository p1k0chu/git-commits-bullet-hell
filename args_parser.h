#pragma once

typedef struct {
    const char *path; // path to git repo
} app_arguments;

app_arguments parse_args(int argc, char **argv);

