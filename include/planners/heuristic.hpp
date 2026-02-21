#pragma once

#include <cmath>
#include <cstdlib>

namespace pbs {

enum class HeuristicType { Manhattan, Euclidean, Diagonal };

inline double heuristic_manhattan(int r0, int c0, int r1, int c1) {
  return std::abs(r1 - r0) + std::abs(c1 - c0);
}

inline double heuristic_euclidean(int r0, int c0, int r1, int c1) {
  int dr = r1 - r0, dc = c1 - c0;
  return std::sqrt(static_cast<double>(dr * dr + dc * dc));
}

inline double heuristic_diagonal(int r0, int c0, int r1, int c1) {
  int dr = std::abs(r1 - r0), dc = std::abs(c1 - c0);
  return std::max(dr, dc) + (M_SQRT2 - 1.0) * std::min(dr, dc);
}

}  // namespace pbs
