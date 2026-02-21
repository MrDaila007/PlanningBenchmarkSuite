#include "environment/grid_environment.hpp"

namespace pbs {

GridEnvironment::GridEnvironment(int width, int height)
  : width_(width), height_(height),
    occupancy_(height, std::vector<int>(width, 0)) {}

bool GridEnvironment::is_valid(const State& s) const {
  if (s.grid_pos) {
    auto [r, c] = *s.grid_pos;
    return r >= 0 && r < height_ && c >= 0 && c < width_ &&
           !occupied(r, c);
  }
  int c = static_cast<int>(s.x), r = static_cast<int>(s.y);
  return r >= 0 && r < height_ && c >= 0 && c < width_ &&
         !occupied(r, c);
}

bool GridEnvironment::collision_free(const State& a, const State& b) const {
  (void)a;
  (void)b;
  // Stub: to be implemented with Bresenham in task 2.1
  return true;
}

double GridEnvironment::clearance(const State& /*s*/) const {
  return 0.0;  // Stub
}

bool GridEnvironment::occupied(int row, int col) const {
  if (row < 0 || row >= height_ || col < 0 || col >= width_)
    return true;
  return occupancy_[row][col] != 0;
}

}  // namespace pbs
