#pragma once

#include <optional>
#include <utility>
#include <cmath>

namespace pbs {

struct State {
  double x = 0.0;
  double y = 0.0;
  std::optional<double> theta;
  std::optional<std::pair<int, int>> grid_pos;

  State() = default;
  State(double x_, double y_) : x(x_), y(y_) {}
  State(double x_, double y_, double theta_) : x(x_), y(y_), theta(theta_) {}
  State(int row, int col)
    : x(static_cast<double>(col)), y(static_cast<double>(row)),
      grid_pos(std::pair{row, col}) {}

  bool operator==(const State& o) const {
    return x == o.x && y == o.y &&
           theta == o.theta && grid_pos == o.grid_pos;
  }
  bool operator!=(const State& o) const { return !(*this == o); }
};

inline double distance(const State& a, const State& b) {
  return std::hypot(b.x - a.x, b.y - a.y);
}

}  // namespace pbs
