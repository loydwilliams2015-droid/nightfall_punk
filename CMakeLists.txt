cmake_minimum_required(VERSION 3.20)
project(nightfall_punk VERSION 0.7 LANGUAGES C)

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
    FetchContent_Declare(enet
        GIT_REPOSITORY https://github.com/lsalzman/enet.git
        GIT_TAG v1.3.18
        GIT_SHALLOW TRUE)
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
    message(WARNING "nightfall: libsodium not found; v0.7 localhost testing will use non-cryptographic session-token scaffolding. Install libsodium-dev for authenticated token derivation.")
endif()

add_library(nightfall_shared STATIC
    src/shared/nf_world.c
    src/shared/nf_movement.c
    src/shared/nf_combat.c
    src/shared/nf_agent.c
    src/shared/nf_relations.c
    src/shared/nf_semantics.c
    src/shared/nf_region.c)
target_include_directories(nightfall_shared PUBLIC src/shared)
nf_project_warnings(nightfall_shared)
if(UNIX)
    target_link_libraries(nightfall_shared PUBLIC m)
endif()

add_library(nightfall_ai STATIC
    src/server/ai/nf_ai.c
    src/server/ai/nf_encounter.c
    src/server/ai/nf_spatial.c
    src/server/ai/nf_spatial_filter.c)
target_include_directories(nightfall_ai PUBLIC src/server/ai src/shared)
target_link_libraries(nightfall_ai PUBLIC nightfall_shared)
nf_project_warnings(nightfall_ai)

add_library(nightfall_network STATIC
    src/shared/nf_protocol.c
    src/shared/nf_prediction.c
    src/shared/nf_security.c
    src/shared/nf_net.c)
target_include_directories(nightfall_network PUBLIC src/shared)
if(NF_ENET_FETCH_INCLUDE)
    target_include_directories(nightfall_network PRIVATE "${NF_ENET_FETCH_INCLUDE}")
endif()
target_link_libraries(nightfall_network PUBLIC nightfall_shared ${NF_ENET_TARGET})
nf_project_warnings(nightfall_network)
if(NF_SODIUM_TARGET)
    target_link_libraries(nightfall_network PUBLIC ${NF_SODIUM_TARGET})
    target_compile_definitions(nightfall_network PUBLIC NF_HAVE_SODIUM=1)
endif()

if(NF_BUILD_SERVER)
    add_executable(nightfall_server src/server/main.c)
    target_link_libraries(nightfall_server PRIVATE nightfall_network nightfall_ai)
    target_include_directories(nightfall_server PRIVATE src/server/ai)
    nf_project_warnings(nightfall_server)
endif()

if(NF_BUILD_NETBOT)
    add_executable(nightfall_netbot src/tools/netbot.c)
    target_link_libraries(nightfall_netbot PRIVATE nightfall_network)
    nf_project_warnings(nightfall_netbot)
endif()

if(NF_BUILD_CLIENT)
    find_package(raylib 5.5 QUIET)
    if(NOT raylib_FOUND AND NF_FETCH_RAYLIB)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.5
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(raylib)
    endif()
    if(NOT TARGET raylib)
        message(FATAL_ERROR "raylib 5.5 was not found. Install raylib or configure with -DNF_FETCH_RAYLIB=ON.")
    endif()
    add_executable(nightfall_client src/client/main.c)
    target_link_libraries(nightfall_client PRIVATE nightfall_network raylib)
    nf_project_warnings(nightfall_client)
endif()

if(NF_BUILD_TESTS)
    enable_testing()
    add_executable(nightfall_world_test src/tests/test_world.c)
    target_link_libraries(nightfall_world_test PRIVATE nightfall_shared)
    nf_project_warnings(nightfall_world_test)
    add_test(NAME simulation_v07 COMMAND nightfall_world_test)

    add_executable(nightfall_combat_test src/tests/test_combat.c)
    target_link_libraries(nightfall_combat_test PRIVATE nightfall_shared)
    nf_project_warnings(nightfall_combat_test)
    add_test(NAME combat_contract_v07 COMMAND nightfall_combat_test)

    add_executable(nightfall_network_test src/tests/test_network.c)
    target_link_libraries(nightfall_network_test PRIVATE nightfall_network)
    nf_project_warnings(nightfall_network_test)
    add_test(NAME network_contract_v07 COMMAND nightfall_network_test)

    add_executable(nightfall_ai_test src/tests/test_ai.c)
    target_link_libraries(nightfall_ai_test PRIVATE nightfall_ai)
    target_include_directories(nightfall_ai_test PRIVATE src/server/ai)
    nf_project_warnings(nightfall_ai_test)
    add_test(NAME spatial_ecology_v07 COMMAND nightfall_ai_test)
endif()
