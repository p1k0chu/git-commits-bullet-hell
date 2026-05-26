// SPDX-License-Identifier: MIT
// Copyright (C) 2026 p1k0chu

#include "main.hpp"

#include <format>
#include <git2/errors.h>
#include <git2/repository.h>
#include <git2/revwalk.h>
#include <optional>
#include <stddef.h>
#include <stdexcept>
#include <stdio.h>
#include <string>

const char *strnchr(const char *s, int c, size_t n) {
    const char *ptr;
    for (ptr = s; ptr < s + n; ptr++) {
        if (*ptr == c)
            return ptr;
    }
    return (const char *)nullptr;
}

void libgit_panic(int error) {
    const git_error *s = git_error_last();
    throw std::runtime_error(std::format("libgit2 error: {}/{}: {}", error, s->klass, s->message));
}

std::optional<Wrappers::Commit> get_next_commit() {
    if (walker == nullptr) {
        return {};
    }
    git_oid oid;
    int code;
    if ((code = git_revwalk_next(&oid, walker)) == GIT_ITEROVER) {
        git_revwalk_free(walker);
        walker = nullptr;
        return {};
    } else if (code != 0) {
        libgit_panic(code);
    }

    git_commit *commit;
    code = git_commit_lookup(&commit, repo, &oid);
    if (code < 0)
        libgit_panic(code);

    return commit;
}

