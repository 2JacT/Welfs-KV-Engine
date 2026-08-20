cmake_minimum_required(VERSION 4.4.1)
project(Welfs_KV_Engine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(xxHash CONFIG REQUIRED)
add_executable(KVEngine src/main.cpp)
target_include_directories(KVEngine PRIVATE include)
target_link_libraries(KVEngine PRIVATE xxHash::xxhash)

add_custom_target(vtune-profile
    COMMAND vtune -collect hotspots
            -result-dir "${CMAKE_BINARY_DIR}/vtune-hotspots-$<CONFIG>"
            -- $<TARGET_FILE:KVEngine>
    DEPENDS KVEngine
    COMMENT "Running VTune hotspot profiling (results: ${CMAKE_BINARY_DIR}/vtune-hotspots-$<CONFIG>)"
    USES_TERMINAL
)

enable_testing()
find_package(GTest CONFIG REQUIRED)

add_executable(workload_test tests/workload_test.cpp)
target_include_directories(workload_test PRIVATE include tests)
target_compile_definitions(workload_test PRIVATE
    WORKLOAD_CONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/tests/config/workload.cfg")
target_link_libraries(workload_test PRIVATE GTest::gtest_main xxHash::xxhash)

include(GoogleTest)
gtest_discover_tests(workload_test)