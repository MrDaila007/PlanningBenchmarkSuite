#include "environment/map_generator.hpp"
#include <algorithm>
#include <random>
#include <utility>
#include <vector>

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
    case MapGeneratorType::Maze:
      return generate_maze(params.width, params.height, rng);
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

namespace {

// Union-Find for Kruskal's algorithm.
class UnionFind {
 public:
  explicit UnionFind(int n) : parent_(n), rank_(n, 0) {
    for (int i = 0; i < n; ++i) parent_[i] = i;
  }
  int find(int x) {
    if (parent_[x] != x) parent_[x] = find(parent_[x]);
    return parent_[x];
  }
  void unite(int a, int b) {
    a = find(a);
    b = find(b);
    if (a == b) return;
    if (rank_[a] < rank_[b])
      parent_[a] = b;
    else if (rank_[a] > rank_[b])
      parent_[b] = a;
    else {
      parent_[b] = a;
      ++rank_[a];
    }
  }

 private:
  std::vector<int> parent_;
  std::vector<int> rank_;
};

}  // namespace

GridEnvironment MapGenerator::generate_maze(int cells_wide, int cells_high,
                                           std::mt19937& rng) {
  // For W x H cells, occupancy grid is (2*H+1) x (2*W+1).
  // Rooms at odd indices (2r+1, 2c+1); walls between rooms.
  const int grid_h = 2 * cells_high + 1;
  const int grid_w = 2 * cells_wide + 1;

  std::vector<std::vector<int>> occ(grid_h, std::vector<int>(grid_w, 1));
  for (int r = 0; r < cells_high; ++r)
    for (int c = 0; c < cells_wide; ++c)
      occ[2 * r + 1][2 * c + 1] = 0;  // Room cells free

  // Edges: wall between adjacent cells. (r,c) - (r+1,c) or (r,c) - (r,c+1)
  using Edge = std::pair<int, int>;  // cell indices
  std::vector<Edge> edges;
  const int num_cells = cells_wide * cells_high;
  for (int r = 0; r < cells_high; ++r) {
    for (int c = 0; c < cells_wide; ++c) {
      int idx = r * cells_wide + c;
      if (r + 1 < cells_high)
        edges.emplace_back(idx, (r + 1) * cells_wide + c);
      if (c + 1 < cells_wide)
        edges.emplace_back(idx, r * cells_wide + (c + 1));
    }
  }

  std::shuffle(edges.begin(), edges.end(), rng);

  UnionFind uf(num_cells);
  for (const auto& [a, b] : edges) {
    if (uf.find(a) != uf.find(b)) {
      uf.unite(a, b);
      // Remove wall between cells a and b.
      int ra = a / cells_wide;
      int ca = a % cells_wide;
      int rb = b / cells_wide;
      int cb = b % cells_wide;
      // Wall cell: between (ra,ca) and (rb,cb)
      if (rb == ra + 1)  // vertical adjacency
        occ[2 * ra + 2][2 * ca + 1] = 0;
      else  // cb == ca + 1, horizontal adjacency
        occ[2 * ra + 1][2 * ca + 2] = 0;
    }
  }

  return GridEnvironment(grid_w, grid_h, std::move(occ));
}

}  // namespace pbs
