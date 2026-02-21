add_executable(test_structure test_structure.cpp)
target_link_libraries(test_structure PRIVATE planning_benchmark planners
  GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(test_structure)
