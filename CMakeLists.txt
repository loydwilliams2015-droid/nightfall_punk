cmake_minimum_required(VERSION 3.20)
project(nightfall_punk VERSION 1.6.2 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(NF_BUILD_CLIENT "Build the raylib client" ON)
option(NF_BUILD_SERVER "Build the dedicated server" ON)
option(NF_BUILD_NETBOT "Build automated network test client" ON)
option(NF_BUILD_TESTS "Build tests" ON)
option(NF_FETCH_RAYLIB "Fetch raylib when no system package is found" ON)
option(NF_FETCH_ENET "Fetch ENet 1.3.18 when no system package is found" ON)

function(nf_project_warnings target_name)
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endfunction()

include(FetchContent)
find_package(PkgConfig QUIET)

set(NF_ENET_TARGET "")
if(PkgConfig_FOUND)
    pkg_check_modules(ENET QUIET IMPORTED_TARGET libenet)
endif()
if(TARGET PkgConfig::ENET)
    set(NF_ENET_TARGET PkgConfig::ENET)
elseif(NF_FETCH_ENET)
    FetchContent_Declare(enet GIT_REPOSITORY https://github.com/lsalzman/enet.git GIT_TAG v1.3.18 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(enet)
    if(UNIX AND TARGET enet)
        set_property(TARGET enet PROPERTY C_EXTENSIONS ON)
        target_compile_definitions(enet PRIVATE _DEFAULT_SOURCE)
    endif()
    set(NF_ENET_TARGET enet)
    set(NF_ENET_FETCH_INCLUDE "${enet_SOURCE_DIR}/include")
else()
    message(FATAL_ERROR "ENet was not found. Install libenet-dev or configure with -DNF_FETCH_ENET=ON.")
endif()

set(NF_SODIUM_TARGET "")
if(PkgConfig_FOUND)
    pkg_check_modules(SODIUM QUIET IMPORTED_TARGET libsodium)
endif()
if(TARGET PkgConfig::SODIUM)
    set(NF_SODIUM_TARGET PkgConfig::SODIUM)
    message(STATUS "nightfall: libsodium enabled")
else()
    message(WARNING "nightfall: libsodium not found; localhost testing will use non-cryptographic session-token scaffolding. Install libsodium-dev for authenticated token derivation.")
endif()

add_library(nightfall_shared STATIC
    src/shared/nf_world.c
    src/shared/nf_movement.c
    src/shared/nf_combat.c
    src/shared/nf_weapon_solution.c
    src/shared/nf_weapon_geometry.c
    src/shared/nf_contamination.c
    src/shared/nf_agent.c
    src/shared/nf_relations.c