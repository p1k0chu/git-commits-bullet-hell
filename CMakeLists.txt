cmake_minimum_required(VERSION 3.16)
project(GitCommitsBulletHell VERSION 0.0.0)

set(CMAKE_C_STANDARD 90)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(MSVC)
    if (DEFINED ENV{VCPKG_ROOT})
        set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
    else()
        if (NOT DEFINED ACTUAL_INCLUDE_DIR AND NOT DEFINED ACTUAL_LINK_DIR)
            message(FATAL_ERROR "Environment variable VCPKG_ROOT is empty! Set VCPKG_ROOT or set both ACTUAL_INCLUDE_DIR and ACTUAL_LINK_DIR")
        endif()
    endif()
    if (NOT DEFINED ACTUAL_INCLUDE_DIR)
        set(ACTUAL_INCLUDE_DIR "${VCPKG_ROOT}\\installed\\x64-windows\\include")
    endif()
    if (NOT DEFINED ACTUAL_LINK_DIR)
        set(ACTUAL_LINK_DIR "${VCPKG_ROOT}\\installed\\x64-windows\\lib")
    endif()
    include_directories(${ACTUAL_INCLUDE_DIR})
    link_directories(${ACTUAL_LINK_DIR})
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(default_build_type "Debug")
    message(STATUS "Set the build type to `${default_build_type}` as none was specified.")
    set(CMAKE_BUILD_TYPE ${default_build_type} CACHE STRING "Chooce the build type." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "${PROJECT_NAME} Build Type: ${CMAKE_BUILD_TYPE}")

find_package(Git REQUIRED)
execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=0
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (GIT_VERSION)
    set(PROJECT_VERSION ${GIT_VERSION})
endif()

message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

if (NOT MSVC)
    find_package(SDL3 REQUIRED)
    find_package(SDL3_ttf REQUIRED)
    find_package(SDL3_image REQUIRED)
endif()

configure_file(version.h.in include/version.h)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include")

add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/include/font.h"
    COMMAND xxd -i -n tiny_ttf ${CMAKE_SOURCE_DIR}/font/tiny.ttf > "${CMAKE_BINARY_DIR}/include/font.h"
    DEPENDS font/tiny.ttf
)
add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/include/heart.h"
    COMMAND xxd -i -n heart_png ${CMAKE_SOURCE_DIR}/assets/heart.png > "${CMAKE_BINARY_DIR}/include/heart.h"
    DEPENDS assets/heart.png
)
add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/include/broken_heart.h"
    COMMAND xxd -i -n broken_heart_png ${CMAKE_SOURCE_DIR}/assets/broken_heart.png > "${CMAKE_BINARY_DIR}/include/broken_heart.h"
    DEPENDS assets/broken_heart.png
)

add_executable(git-commits-bullet-hell main.c)
target_sources(git-commits-bullet-hell PRIVATE
    buffer.c
    enemy.c
    player.c
    pattern.c
    utils.c
    math.c
    "${CMAKE_BINARY_DIR}/include/font.h"
    "${CMAKE_BINARY_DIR}/include/heart.h"
    "${CMAKE_BINARY_DIR}/include/broken_heart.h"
)
target_link_libraries(git-commits-bullet-hell PRIVATE
    SDL3
    SDL3_ttf
    SDL3_image
)
target_include_directories(git-commits-bullet-hell PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
)

install(
    TARGETS git-commits-bullet-hell
    DESTINATION bin
)

