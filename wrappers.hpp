#pragma once

#include <git2/commit.h>
#include <string_view>

namespace Wrappers {

/// @brief Wrapper around git_commit that manages the resource.
class Commit {
  public:
    Commit(git_commit* commit);
    ~Commit();

    Commit(const Commit&) = delete;
    Commit& operator=(const Commit&) = delete;

    Commit(Commit&&);
    Commit& operator=(Commit&&);

    const char* summary();
    std::string_view sha_str() const;
    bool has_value() const;

  private:
    git_commit* commit;
    const char* _summary = nullptr;
};

} // namespace Wrappers
