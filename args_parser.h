// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#pragma once

typedef struct {
    const char *path; // path to git repo
    const char *rev_push;
    const char *rev_hide;
} app_arguments;

app_arguments parse_args(int argc, char **argv);

