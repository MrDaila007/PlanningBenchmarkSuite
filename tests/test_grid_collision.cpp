#include <gtest/gtest.h>
#include "geometry/grid_collision_checker.hpp"
#include "environment/grid_environment.hpp"

namespace {

TEST(GridCollisionTest, IsOccupiedOnEmptyMap) {
  std::vector<std::vector<int>> occ(10, std::vector<int>(10, 0));
  pbs::GridCollisionChecker checker(10, 10, occ);
  EXPECT_FALSE(checker.is_occupied(0, 0));
  EXPECT_FALSE(checker.is_occupied(5, 5));
}

TEST(GridCollisionTest, IsOccupiedWithObstacle) {
  std::vector<std::vector<int>> occ(10, std::vector<int>(10, 0));
  occ[3][3] = 1;
  pbs::GridCollisionChecker checker(10, 10, occ);
  EXPECT_TRUE(checker.is_occupied(3, 3));
}

TEST(GridCollisionTest, LineOfSightFree) {
  std::vector<std::vector<int>> occ(10, std::vector<int>(10, 0));
  pbs::GridCollisionChecker checker(10, 10, occ);
  EXPECT_TRUE(checker.line_of_sight(0, 0, 5, 5));
}

TEST(GridCollisionTest, LineOfSightBlocked) {
  std::vector<std::vector<int>> occ(10, std::vector<int>(10, 0));
  occ[2][2] = 1;
  pbs::GridCollisionChecker checker(10, 10, occ);
  EXPECT_FALSE(checker.line_of_sight(0, 0, 4, 4));
}

TEST(GridCollisionTest, LineOfSightHorizontalVertical) {
  std::vector<std::vector<int>> occ(10, std::vector<int>(10, 0));
  pbs::GridCollisionChecker checker(10, 10, occ);
  EXPECT_TRUE(checker.line_of_sight(0, 0, 9, 0));
  EXPECT_TRUE(checker.line_of_sight(0, 0, 0, 9));
}

TEST(GridCollisionTest, GridEnvironmentCollisionFree) {
  pbs::GridEnvironment env(10, 10);
  EXPECT_TRUE(env.collision_free(pbs::State(0, 0), pbs::State(9, 9)));
}

TEST(GridCollisionTest, GridEnvironmentCollisionBlocked) {
  std::vector<std::vector<int>> occ(5, std::vector<int>(5, 0));
  occ[2][2] = 1;
  pbs::GridEnvironment env(5, 5, std::move(occ));
  EXPECT_FALSE(env.collision_free(pbs::State(0, 0), pbs::State(4, 4)));
}

}  // namespace
