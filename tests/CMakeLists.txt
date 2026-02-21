add_executable(test_structure
  test_structure.cpp
  test_core.cpp
  test_grid_collision.cpp
  test_map_generator.cpp
  test_planners.cpp
)
target_link_libraries(test_structure PRIVATE planning_benchmark planners
  GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(test_structure)
