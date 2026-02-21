#include <gtest/gtest.h>
#include "core/state.hpp"
#include "core/path.hpp"
#include "core/iplanner.hpp"
#include "environment/ienvironment.hpp"
#include "environment/grid_environment.hpp"
#include "planners/dijkstra.hpp"

namespace {

TEST(StructureTest, StateStoresXY) {
  pbs::State s(1.0, 2.0);
  EXPECT_DOUBLE_EQ(s.x, 1.0);
  EXPECT_DOUBLE_EQ(s.y, 2.0);
}

TEST(StructureTest, DijkstraFindsPathOnEmptyGrid) {
  pbs::GridEnvironment env(10, 10);
  pbs::DijkstraPlanner planner;
  pbs::Path path =
      planner.solve(env, pbs::State(0, 0), pbs::State(9, 9));
  EXPECT_TRUE(path.success);
  EXPECT_FALSE(path.states.empty());
  EXPECT_GE(path.length, 9.0);
}

TEST(StructureTest, PathEmpty) {
  pbs::Path p;
  EXPECT_TRUE(p.empty());
  EXPECT_FALSE(p.success);
}

}  // namespace
