cmake_minimum_required(VERSION 3.20)
project(nightfall_punk VERSION 1.4 LANGUAGES C)

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
    message(WARNING "nightfall: libsodium not found; localhost testing will use non-cryptographic session-token scaffolding. Install libsodium-dev for authenticated token derivation.")
endif()

add_library(nightfall_shared STATIC
    src/shared/nf_world.c
    src/shared/nf_movement.c
    src/shared/nf_combat.c
    src/shared/nf_contamination.c
    src/shared/nf_agent.c
    src/shared/nf_relations.c
    src/shared/nf_semantics.c
    src/shared/nf_report.c
    src/shared/nf_claim.c
    src/shared/nf_region.c
    src/shared/nf_energy.c
    src/shared/nf_energy_actor.c
    src/shared/nf_hitbox.c
    src/shared/nf_lifeworld.c
    src/shared/nf_route.c)
target_include_directories(nightfall_shared PUBLIC src/shared)
nf_project_warnings(nightfall_shared)
if(UNIX)
    target_link_libraries(nightfall_shared PUBLIC m)
endif()

# Presentation-only camera math. It consumes body/world truth but cannot mutate simulation.
add_library(nightfall_camera STATIC
    src/client/nf_camera.c)
target_include_directories(nightfall_camera PUBLIC src/client src/shared)
target_link_libraries(nightfall_camera PUBLIC nightfall_shared)
nf_project_warnings(nightfall_camera)

add_library(nightfall_ai STATIC
    src/server/ai/nf_ai.c
    src/server/ai/nf_encounter.c
    src/server/ai/nf_spatial.c
    src/server/ai/nf_spatial_filter.c
    src/server/ai/nf_spatial_attention.c
    src/server/ai/nf_cattler.c
    src/server/ai/nf_cattler_recurrence.c
    src/server/ai/nf_lifeworld_spatial.c
    src/server/ai/nf_teammate.c)
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
    target_link_libraries(nightfall_client PRIVATE nightfall_camera nightfall_network raylib)
    target_include_directories(nightfall_client PRIVATE src/client)
    nf_project_warnings(nightfall_client)

    add_executable(nightfall_v12_demo src/demo/v12_greybox.c)
    target_link_libraries(nightfall_v12_demo PRIVATE nightfall_camera nightfall_ai raylib)
    target_include_directories(nightfall_v12_demo PRIVATE src/client src/server/ai src/shared)
    nf_project_warnings(nightfall_v12_demo)

    add_executable(nightfall_v14_demo src/demo/v14_greybox.c)
    target_link_libraries(nightfall_v14_demo PRIVATE nightfall_camera nightfall_ai raylib)
    target_include_directories(nightfall_v14_demo PRIVATE src/client src/server/ai src/shared)
    nf_project_warnings(nightfall_v14_demo)
endif()

if(NF_BUILD_TESTS)
    enable_testing()
    add_executable(nightfall_world_test src/tests/test_world.c)
    target_link_libraries(nightfall_world_test PRIVATE nightfall_shared)
    nf_project_warnings(nightfall_world_test)
    add_test(NAME simulation_v11 COMMAND nightfall_world_test)

    add_executable(nightfall_combat_test src/tests/test_combat.c)
    target_link_libraries(nightfall_combat_test PRIVATE nightfall_shared)
    nf_project_warnings(nightfall_combat_test)
    add_test(NAME combat_contract_v11 COMMAND nightfall_combat_test)

    add_executable(nightfall_contamination_test src/tests/test_contamination.c)
    target_link_libraries(nightfall_contamination_test PRIVATE nightfall_shared)
    nf_project_warnings(nightfall_contamination_test)
    add_test(NAME contamination_contract_v11 COMMAND nightfall_contamination_test)

    add_executable(nightfall_network_test src/tests/test_network.c)
    target_link_libraries(nightfall_network_test PRIVATE nightfall_network)
    nf_project_warnings(nightfall_network_test)
    add_test(NAME network_contract_v11 COMMAND nightfall_network_test)

    add_executable(nightfall_ai_test src/tests/test_ai.c)
    target_link_libraries(nightfall_ai_test PRIVATE nightfall_ai)
    target_include_directories(nightfall_ai_test PRIVATE src/server/ai)
    nf_project_warnings(nightfall_ai_test)
    add_test(NAME dream_cattler_ecology_v11 COMMAND nightfall_ai_test)

    add_executable(nightfall_energy_test src/tests/test_energy.c)
    target_link_libraries(nightfall_energy_test PRIVATE nightfall_shared)
    nf_project_warnings(nightfall_energy_test)
    add_test(NAME topographical_energy_v11 COMMAND nightfall_energy_test)

    add_executable(nightfall_cattler_weakpoint_test src/tests/test_cattler_weakpoint.c)
    target_link_libraries(nightfall_cattler_weakpoint_test PRIVATE nightfall_ai)
    target_include_directories(nightfall_cattler_weakpoint_test PRIVATE src/server/ai)
    nf_project_warnings(nightfall_cattler_weakpoint_test)
    add_test(NAME cattler_weakpoint_truth_v11 COMMAND nightfall_cattler_weakpoint_test)

    add_executable(nightfall_lifeworld_test src/tests/test_lifeworld.c)
    target_link_libraries(nightfall_lifeworld_test PRIVATE nightfall_shared)
    nf_project_warnings(nightfall_lifeworld_test)
    add_test(NAME topographic_lifeworld_v11 COMMAND nightfall_lifeworld_test)

    add_executable(nightfall_lifeworld_runtime_test src/tests/test_lifeworld_runtime.c)
    target_link_libraries(nightfall_lifeworld_runtime_test PRIVATE nightfall_ai)
    target_include_directories(nightfall_lifeworld_runtime_test PRIVATE src/server/ai)
    nf_project_warnings(nightfall_lifeworld_runtime_test)
    add_test(NAME lifeworld_runtime_v11 COMMAND nightfall_lifeworld_runtime_test)

    add_executable(nightfall_teammate_test src/tests/test_teammate.c)
    target_link_libraries(nightfall_teammate_test PRIVATE nightfall_ai)
    target_include_directories(nightfall_teammate_test PRIVATE src/server/ai src/shared)
    nf_project_warnings(nightfall_teammate_test)
    add_test(NAME teammate_causality_v12 COMMAND nightfall_teammate_test)

    add_executable(nightfall_camera_test src/tests/test_camera.c)
    target_link_libraries(nightfall_camera_test PRIVATE nightfall_camera)
    target_include_directories(nightfall_camera_test PRIVATE src/client src/shared)
    nf_project_warnings(nightfall_camera_test)
    add_test(NAME relational_camera_v12b COMMAND nightfall_camera_test)

    add_executable(nightfall_v14_relations_test src/tests/test_v14_relations.c)
    target_link_libraries(nightfall_v14_relations_test PRIVATE nightfall_ai)
    target_include_directories(nightfall_v14_relations_test PRIVATE src/server/ai src/shared)
    nf_project_warnings(nightfall_v14_relations_test)
    add_test(NAME relational_comms_claims_v14 COMMAND nightfall_v14_relations_test)
endif()
