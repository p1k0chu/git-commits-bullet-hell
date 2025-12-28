cmake_minimum_required(VERSION 3.16)
project(GitCommitsBulletHell VERSION 0.0.0)

set(CMAKE_C_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra)

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

find_package(SDL3 REQUIRED)

configure_file(version.h.in version.h)

add_executable(main main.c)
target_sources(main PRIVATE
    buffer.c
    utils.c
)
target_link_libraries(main PRIVATE SDL3)
target_include_directories(main PRIVATE ${CMAKE_BINARY_DIR})

