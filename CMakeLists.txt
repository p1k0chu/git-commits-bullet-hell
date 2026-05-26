cmake_minimum_required(VERSION 3.16)
project(GitCommitsBulletHell VERSION 0.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT MSVC)
    add_compile_options(-Wall -Wextra -Werror=reorder)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(default_build_type "Debug")
    message(
        STATUS
        "Set the build type to `${default_build_type}` as none was specified."
    )
    set(CMAKE_BUILD_TYPE
        ${default_build_type}
        CACHE STRING
        "Chooce the build type."
        FORCE
    )
    set_property(
        CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo"
    )
endif()
message(STATUS "${PROJECT_NAME} Build Type: ${CMAKE_BUILD_TYPE}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GitVersion)
if(GIT_VERSION)
    set(PROJECT_VERSION ${GIT_VERSION})
endif()

message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(libgit2 CONFIG REQUIRED)

configure_file(version.h.in include/version.h)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include")

add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/include/font.h"
    COMMAND
        xxd -i -n tiny_ttf ${CMAKE_SOURCE_DIR}/font/tiny.ttf >
        "${CMAKE_BINARY_DIR}/include/font.h"
    DEPENDS font/tiny.ttf
)
add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/include/heart.h"
    COMMAND
        xxd -i -n heart_png ${CMAKE_SOURCE_DIR}/assets/heart.png >
        "${CMAKE_BINARY_DIR}/include/heart.h"
    DEPENDS assets/heart.png
)
add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/include/broken_heart.h"
    COMMAND
        xxd -i -n broken_heart_png ${CMAKE_SOURCE_DIR}/assets/broken_heart.png >
        "${CMAKE_BINARY_DIR}/include/broken_heart.h"
    DEPENDS assets/broken_heart.png
)

add_executable(
    git-commits-bullet-hell
    main.cpp
    enemy.cpp
    enemy_behaviour.cpp
    player.cpp
    pattern.cpp
    pattern_factory.cpp
    utils.cpp
    wrappers.cpp
    math.cpp
    args_parser.cpp
    "${CMAKE_BINARY_DIR}/include/font.h"
    "${CMAKE_BINARY_DIR}/include/heart.h"
    "${CMAKE_BINARY_DIR}/include/broken_heart.h"
)
target_link_libraries(
    git-commits-bullet-hell
    PRIVATE SDL3 SDL3_ttf SDL3_image git2
)
target_include_directories(
    git-commits-bullet-hell
    PRIVATE ${CMAKE_BINARY_DIR}/include ${CMAKE_SOURCE_DIR}
)

install(TARGETS git-commits-bullet-hell DESTINATION bin)
