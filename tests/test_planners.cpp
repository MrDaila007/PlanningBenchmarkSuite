#include <gtest/gtest.h>
#include "planners/dijkstra.hpp"
#include "planners/astar.hpp"
#include "planners/heuristic.hpp"
#include "environment/grid_environment.hpp"

namespace {

TEST(PlannersTest, DijkstraEmptyMap) {
  pbs::GridEnvironment env(10, 10);
  pbs::DijkstraPlanner planner;
  auto path = planner.solve(env, pbs::State(0, 0), pbs::State(9, 9));
  EXPECT_TRUE(path.success);
  EXPECT_GE(path.states.size(), 2u);
  EXPECT_NEAR(path.length, 9.0 * M_SQRT2, 1.0);
  EXPECT_GT(planner.nodes_expanded(), 0);
}

TEST(PlannersTest, DijkstraWithObstacle) {
  std::vector<std::vector<int>> occ(10, std::vector<int>(10, 0));
  for (int c = 1; c < 9; ++c)
    occ[5][c] = 1;  // Wall in middle, (5,0) and (5,9) free for passage
  pbs::GridEnvironment env(10, 10, std::move(occ));
  pbs::DijkstraPlanner planner;
  auto path = planner.solve(env, pbs::State(0, 0), pbs::State(9, 9));
  EXPECT_TRUE(path.success);
  EXPECT_GT(path.length, 12.0);  // Longer than straight diagonal ~12.7
}

TEST(PlannersTest, AStarFindsPath) {
  pbs::GridEnvironment env(10, 10);
  pbs::AStarPlanner planner(pbs::HeuristicType::Diagonal);
  auto path = planner.solve(env, pbs::State(0, 0), pbs::State(9, 9));
  EXPECT_TRUE(path.success);
  EXPECT_GT(planner.nodes_expanded(), 0);
}

TEST(PlannersTest, AStarVsDijkstraExpansions) {
  pbs::GridEnvironment env(20, 20);
  pbs::DijkstraPlanner dijkstra;
  pbs::AStarPlanner astar(pbs::HeuristicType::Diagonal);
  auto p1 = dijkstra.solve(env, pbs::State(0, 0), pbs::State(19, 19));
  auto p2 = astar.solve(env, pbs::State(0, 0), pbs::State(19, 19));
  EXPECT_TRUE(p1.success);
  EXPECT_TRUE(p2.success);
  EXPECT_LE(astar.nodes_expanded(), dijkstra.nodes_expanded());
}

}  // namespace
