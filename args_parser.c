// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#include "args_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_help(char *program_name);

app_arguments parse_args(int argc, char **argv) {
    app_arguments args = {0};

    enum {
        NOTHING,
        PATH,
    } parser_state = NOTHING;

    int i;
    for (i = 1; i < argc; ++i) {
        char *arg = argv[i];

        switch (parser_state) {
        case NOTHING:
            if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0)
                print_help(argv[0]);
            if (strcmp(arg, "-p") == 0 || strcmp(arg, "--path") == 0) {
                parser_state = PATH;
            } else if (arg[0] == '^') {
                args.rev_hide = arg + 1;
            } else if (strstr(arg, "..")) {
                // if revision to hide is not present then
                // strtok will return the second token immediately
                if (arg[0] != '.') {
                    args.rev_hide = strtok(arg, "..");
                    arg = NULL;
                }
                args.rev_push = strtok(arg, "..");

                if (strtok(NULL, "..")) {
                    fputs("git revision range can only have two revisions", stderr);
                    exit(1);
                }
            } else {
                args.rev_push = arg;
            }
            break;
        case PATH:
            args.path = arg;
            parser_state = NOTHING;
            break;
        }
    }

    if (args.path == NULL)
        args.path = getenv("PWD");

    return args;
}

static void print_help(char *s) {
    printf("Usage: %s [options] [revision-range]\n\n", s);
    puts(
        "Options:\n"
        "\t-h, --help:\n"
        "\t\tShow help\n"
        "\t-p, --path:\n"
        "\t\tpath to the git repository. uses current directory if omitted");
    exit(0);
}

