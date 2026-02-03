// Copyright (C) 2026 p1k0chu
// SPDX-License-Identifier: MIT

#define _GNU_SOURCE

#include "build.h"

#include <assert.h>
#include <cbuild.h>
#include <cbuild/compile.h>
#include <cbuild/custom_target.h>
#include <cbuild/pkgconfig.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define FOR_EACH_SRC(F) \
    F(args_parser)      \
    F(enemy)            \
    F(main)             \
    F(math)             \
    F(pattern)          \
    F(player)           \
    F(utils)

#define MAKE_ENUM(V) src_##V,
#define MAKE_STRING(V) #V ".c",

enum srcs { FOR_EACH_SRC(MAKE_ENUM) };

const char *srcs[] = {FOR_EACH_SRC(MAKE_STRING)};

const char exe_name[] = "git-commits-bullet-hell";

const char *libs[] = {"sdl3", "sdl3-image", "sdl3-ttf", "libgit2"};
const size_t nlibs = sizeof(libs) / sizeof(*libs);

const char *CFLAGS[] = {
    "-Wall",
    "-Wextra",
    "-Iinclude",
    "-std=gnu90",
    "-DGitCommitsBulletHell_VERSION=\"0.1.0\"", // TODO: run git to get version
};
const size_t NCFLAGS = sizeof(CFLAGS) / sizeof(*CFLAGS);

static void xxd(const char *outpath, const char *inpath);

int main(int argc, char **argv) {
    cbuild_recompile_myself(__FILE__,
                            argv,
                            "-Wall",
                            "-Wextra",
                            "-std=gnu23",
                            "-Wno-unused-parameter",
                            CBUILD_SELFCOMPILE_FLAGS,
                            NULL);

    char **ldlibs;
    int ldlibs_len = cbuild_pkgconfig(&ldlibs, PKG_CONFIG_LIBS, libs, nlibs, NULL);

    char **libflags;
    int libflags_len = cbuild_pkgconfig(&libflags, PKG_CONFIG_CFLAGS, libs, nlibs, NULL);

    cbuild_executable_t *exe = cbuild_create_executable(exe_name, NULL);
    cbuild_executable_append_ldflags(exe, (const char **)ldlibs, ldlibs_len);

    const size_t nobjs = sizeof(srcs) / sizeof(*srcs);
    cbuild_target_t *objs[nobjs];

    for (size_t i = 0; i < nobjs; ++i) {
        cbuild_obj_t *obj = cbuild_obj_create(srcs[i], NULL);
        cbuild_obj_append_cflags(obj, CFLAGS, NCFLAGS);
        cbuild_obj_append_cflags(obj, (const char **)libflags, libflags_len);
        objs[i] = (void *)obj;
    }

    const size_t nxxdts = 3;
    cbuild_target_t *xxd_targets[nxxdts];

    xxd_targets[0] = (void *)cbuild_create_custom_target("font.h", "font/tiny.ttf", xxd);
    xxd_targets[1] =
        (void *)cbuild_create_custom_target("broken_heart.h", "assets/broken_heart.png", xxd);
    xxd_targets[2] = (void *)cbuild_create_custom_target("heart.h", "assets/heart.png", xxd);

    cbuild_target_append_deps(objs[src_main], xxd_targets, nxxdts);
    cbuild_target_append_deps((void *)exe, objs, nobjs);

    pid_t cpid = cbuild_target_compile((void *)exe);
    if (cpid > 0) {
        int ws;
        waitpid(cpid, &ws, 0);
        if (!WIFEXITED(ws))
            errx(1, "compilation died");
        return WEXITSTATUS(ws);
    }
    return 0;
}

static void xxd(const char *outpath, const char *inpath) {
    const char *slash = strchr(inpath, '/');
    if (slash == NULL)
        slash = inpath;
    else
        ++slash;

    const size_t slash_len = strlen(slash);
    char basename[slash_len + 1];
    char *p = mempcpy(basename, slash, slash_len);
    *p = 0;
    for (size_t i = 0; i < slash_len; ++i) {
        if (basename[i] == '.')
            basename[i] = '_';
    }

    const size_t cmdlen = 6;
    char *cmd[cmdlen];
    size_t i = 0;

    cmd[i++] = "xxd";
    cmd[i++] = "-i";
    cmd[i++] = "-n";
    cmd[i++] = basename;
    cmd[i++] = (char *)inpath;
    cmd[i++] = NULL;

    for (size_t j = 0; j < cmdlen - 1; ++j) {
        printf("%s ", cmd[j]);
    }
    printf("> %s\n", outpath);

    int fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
        err(1, "open");

    if (dup2(fd, STDOUT_FILENO) < 0)
        err(1, "dup2");
    if (close(fd) < 0)
        err(1, "close");

    execvp(cmd[0], cmd);
    err(1, "failed to exec xxd");
}
