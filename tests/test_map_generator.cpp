#include <gtest/gtest.h>
#include "environment/map_generator.hpp"

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

}  // namespace
