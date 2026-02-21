#pragma once

#include "environment/grid_environment.hpp"
#include <cstdint>
#include <random>

namespace pbs {

enum class MapGeneratorType {
  RandomUniform,
  PerlinNoise,
  Maze,
  NarrowPassage,
  RandomConvexPolygons
};

struct MapGeneratorParams {
  int width = 10;
  int height = 10;
  double obstacle_density = 0.2;
  int narrow_passage_width = 0;
  double clustering_factor = 0.0;
  uint64_t seed = 0;
  MapGeneratorType type = MapGeneratorType::RandomUniform;
};

class MapGenerator {
 public:
  MapGenerator();
  explicit MapGenerator(uint64_t seed);
  void set_seed(uint64_t seed) { seed_ = seed; }
  GridEnvironment generate(const MapGeneratorParams& params);

 private:
  uint64_t seed_ = 0;
  GridEnvironment generate_random_uniform(int width, int height,
                                         double obstacle_density,
                                         std::mt19937& rng);
};

}  // namespace pbs
