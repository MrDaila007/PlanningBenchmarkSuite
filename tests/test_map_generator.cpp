#include <gtest/gtest.h>
#include "environment/map_generator.hpp"
#include "planners/astar.hpp"

namespace {

TEST(MapGeneratorTest, Determinism) {
  pbs::MapGeneratorParams params{10, 10, 0.3, 0, 0.0, 42,
                                 pbs::MapGeneratorType::RandomUniform};
  pbs::MapGenerator gen;
  auto env1 = gen.generate(params);
  auto env2 = gen.generate(params);
  for (int r = 0; r < 10; ++r)
    for (int c = 0; c < 10; ++c)
      EXPECT_EQ(env1.occupied(r, c), env2.occupied(r, c));
}

TEST(MapGeneratorTest, JSONRoundTrip) {
  pbs::MapGeneratorParams params{5, 5, 0.2, 0, 0.0, 123,
                                 pbs::MapGeneratorType::RandomUniform};
  pbs::MapGenerator gen;
  auto env = gen.generate(params);
  std::string json = env.to_json();
  auto loaded = pbs::GridEnvironment::from_json(json);
  for (int r = 0; r < 5; ++r)
    for (int c = 0; c < 5; ++c)
      EXPECT_EQ(env.occupied(r, c), loaded.occupied(r, c));
}

TEST(MapGeneratorTest, MazeDeterminism) {
  pbs::MapGeneratorParams params{4, 4, 0.0, 0, 0.0, 42,
                                 pbs::MapGeneratorType::Maze};
  pbs::MapGenerator gen;
  auto env1 = gen.generate(params);
  auto env2 = gen.generate(params);
  EXPECT_EQ(env1.width(), 9);   // 2*4+1
  EXPECT_EQ(env1.height(), 9);  // 2*4+1
  for (int r = 0; r < 9; ++r)
    for (int c = 0; c < 9; ++c)
      EXPECT_EQ(env1.occupied(r, c), env2.occupied(r, c));
}

TEST(MapGeneratorTest, MazePerfectConnectivity) {
  // Perfect maze: all cells reachable. 4x4 cells -> 9x9 grid.
  pbs::MapGeneratorParams params{4, 4, 0.0, 0, 0.0, 42,
                                 pbs::MapGeneratorType::Maze};
  pbs::MapGenerator gen;
  auto env = gen.generate(params);
  EXPECT_EQ(env.width(), 9);
  EXPECT_EQ(env.height(), 9);
  EXPECT_FALSE(env.occupied(1, 1));   // start cell free
  EXPECT_FALSE(env.occupied(7, 7));   // goal cell free

  // Verify path exists (perfect maze => single connected component)
  pbs::AStarPlanner planner;
  pbs::State start(1, 1);
  pbs::State goal(7, 7);
  auto path = planner.solve(env, start, goal);
  EXPECT_TRUE(path.success) << "Perfect maze must have path from start to goal";
}

TEST(MapGeneratorTest, MazeJSONRoundTrip) {
  pbs::MapGeneratorParams params{3, 3, 0.0, 0, 0.0, 999,
                                 pbs::MapGeneratorType::Maze};
  pbs::MapGenerator gen;
  auto env = gen.generate(params);
  std::string json = env.to_json();
  auto loaded = pbs::GridEnvironment::from_json(json);
  for (int r = 0; r < env.height(); ++r)
    for (int c = 0; c < env.width(); ++c)
      EXPECT_EQ(env.occupied(r, c), loaded.occupied(r, c));
}

}  // namespace
