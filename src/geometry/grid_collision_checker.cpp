#include "geometry/grid_collision_checker.hpp"
#include <algorithm>
#include <cmath>

namespace pbs {

GridCollisionChecker::GridCollisionChecker(
    int width, int height,
    const std::vector<std::vector<int>>& occupancy)
  : width_(width), height_(height), occupancy_(occupancy) {}

bool GridCollisionChecker::is_occupied(int row, int col) const {
  if (row < 0 || row >= height_ || col < 0 || col >= width_)
    return true;  // Out of bounds treated as occupied
  return occupancy_[row][col] != 0;
}

bool GridCollisionChecker::is_cell_free(int row, int col) const {
  return !is_occupied(row, col);
}

bool GridCollisionChecker::line_of_sight(int r0, int c0, int r1, int c1) const {
  int dr = std::abs(r1 - r0);
  int dc = std::abs(c1 - c0);
  int sr = (r0 < r1) ? 1 : -1;
  int sc = (c0 < c1) ? 1 : -1;

  int r = r0, c = c0;
  if (dc >= dr) {
    int err = 2 * dr - dc;
    for (int i = 0; i <= dc; ++i) {
      if (is_occupied(r, c))
        return false;
      if (r == r1 && c == c1)
        return true;
      if (err > 0) {
        r += sr;
        err -= 2 * dc;
      }
      err += 2 * dr;
      c += sc;
    }
  } else {
    int err = 2 * dc - dr;
    for (int i = 0; i <= dr; ++i) {
      if (is_occupied(r, c))
        return false;
      if (r == r1 && c == c1)
        return true;
      if (err > 0) {
        c += sc;
        err -= 2 * dr;
      }
      err += 2 * dc;
      r += sr;
    }
  }
  return true;
}

}  // namespace pbs
