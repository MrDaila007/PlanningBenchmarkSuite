#pragma once

#include <vector>

namespace pbs {

/// Collision checking for occupancy grids. 0=free, non-zero=occupied.
class GridCollisionChecker {
 public:
  GridCollisionChecker(int width, int height,
                      const std::vector<std::vector<int>>& occupancy);
  bool is_occupied(int row, int col) const;
  bool is_cell_free(int row, int col) const;
  /// Returns true if all cells along Bresenham line from (r0,c0) to (r1,c1)
  /// are free. Returns false if any cell is occupied or out of bounds.
  bool line_of_sight(int r0, int c0, int r1, int c1) const;

 private:
  int width_ = 0;
  int height_ = 0;
  const std::vector<std::vector<int>>& occupancy_;
};

}  // namespace pbs
