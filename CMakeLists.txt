cmake_minimum_required(VERSION 3.20)
project(PlanningBenchmarkSuite VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
# Install: apt install libeigen3-dev nlohmann-json3-dev libgtest-dev cmake
# Or use vcpkg: -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake
find_package(Eigen3 3.3 QUIET NO_MODULE)
find_package(nlohmann_json 3.2 QUIET)
find_package(GTest QUIET)
if(NOT nlohmann_json_FOUND)
  message(FATAL_ERROR "nlohmann_json not found. Install: apt install nlohmann-json3-dev")
endif()
if(NOT GTest_FOUND)
  message(FATAL_ERROR "GTest not found. Install: apt install libgtest-dev; or use FetchContent")
endif()
find_package(Threads REQUIRED)

# Core library
add_library(planning_benchmark
  src/core/state.cpp
  src/core/path.cpp
  src/geometry/grid_collision_checker.cpp
  src/geometry/polygon.cpp
  src/geometry/continuous_collision_checker.cpp
  src/benchmark/benchmark_engine.cpp
  src/benchmark/statistics.cpp
  src/metrics/metrics_collector.cpp
  src/environment/grid_environment_stub.cpp
  src/environment/map_generator.cpp
  src/environment/continuous_environment.cpp
  src/environment/se2_environment.cpp
)
target_include_directories(planning_benchmark
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(planning_benchmark PUBLIC nlohmann_json::nlohmann_json)

# Planners
add_library(planners
  src/planners/dijkstra.cpp
  src/planners/astar.cpp
  src/planners/weighted_astar.cpp
  src/planners/thetastar.cpp
)
target_link_libraries(planners PUBLIC planning_benchmark)
target_include_directories(planners PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Benchmark CLI
add_executable(benchmark apps/benchmark/main.cpp)
target_link_libraries(benchmark PRIVATE planning_benchmark planners nlohmann_json::nlohmann_json)

# Tests
enable_testing()
add_subdirectory(tests)
