#include "environment/map_generator.hpp"
#include <random>

namespace pbs {

MapGenerator::MapGenerator() : seed_(0) {}
MapGenerator::MapGenerator(uint64_t seed) : seed_(seed) {}

GridEnvironment MapGenerator::generate(const MapGeneratorParams& params) {
  uint64_t s = (params.seed != 0) ? params.seed : seed_;
  seed_ = s;  // Update for next call without params
  std::mt19937 rng(static_cast<uint32_t>(s & 0xFFFFFFFFu));
  switch (params.type) {
    case MapGeneratorType::RandomUniform:
      return generate_random_uniform(params.width, params.height,
                                    params.obstacle_density, rng);
    default:
      return generate_random_uniform(params.width, params.height,
                                    params.obstacle_density, rng);
  }
}

GridEnvironment MapGenerator::generate_random_uniform(
    int width, int height, double obstacle_density, std::mt19937& rng) {
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  std::vector<std::vector<int>> occ(height, std::vector<int>(width, 0));
  for (int r = 0; r < height; ++r) {
    for (int c = 0; c < width; ++c) {
      if (r == 0 && c == 0)
        continue;  // Start corner free
      if (r == height - 1 && c == width - 1)
        continue;  // Goal corner free
      if (dist(rng) < obstacle_density)
        occ[r][c] = 1;
    }
  }
  return GridEnvironment(width, height, std::move(occ));
}

}  // namespace pbs
