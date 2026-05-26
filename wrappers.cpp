#include "wrappers.hpp"

#include <format>
#include <git2/errors.h>
#include <stdexcept>
#include <string>

namespace Wrappers {

Commit::Commit(git_commit *commit) : commit(commit) {}

Commit::~Commit() {
    git_commit_free(commit);
}

Commit::Commit(Commit &&other) : commit(other.commit) {
    other.commit = nullptr;
    other._summary = nullptr;
}

Commit &Commit::operator=(Commit &&other) {
    if (this != &other) {
        git_commit_free(this->commit);
        this->_summary = nullptr;

        this->commit = other.commit;
        other.commit = nullptr;
        other._summary = nullptr;
    }
    return *this;
}

const char *Commit::summary() {
    if (_summary == nullptr) {
        _summary = git_commit_summary(commit);
        if (_summary == nullptr) {
            auto err = git_error_last();
            throw std::runtime_error{std::format("libgit error: {}, {}", err->klass, err->message)};
        }
    }
    return _summary;
}

std::string_view Commit::sha_str() const {
    const git_oid *id = git_commit_id(commit);
    return git_oid_tostr_s(id);
}

bool Commit::has_value() const {
    return commit != nullptr;
}

} // namespace Wrappers

