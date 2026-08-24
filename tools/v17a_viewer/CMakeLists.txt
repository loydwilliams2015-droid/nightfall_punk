cmake_minimum_required(VERSION 3.20)
project(nightfall_v17a_viewer LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

include(FetchContent)
find_package(raylib 5.5 QUIET)
if(NOT raylib_FOUND)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(raylib)
endif()

add_executable(nightfall_v17a_viewer
    ../../src/shared/nf_spatial_logic.c
    ../../src/shared/nf_spatial_review.c
    ../../src/demo/v17a_spatial_viewer.c)

target_include_directories(nightfall_v17a_viewer PRIVATE ../../src/shared)
target_compile_options(nightfall_v17a_viewer PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(nightfall_v17a_viewer PRIVATE raylib)
if(UNIX)
    target_link_libraries(nightfall_v17a_viewer PRIVATE m)
endif()
