#pragma once

namespace pbs {

struct Point2D {
  double x = 0.0;
  double y = 0.0;
  Point2D() = default;
  Point2D(double x_, double y_) : x(x_), y(y_) {}
};

}  // namespace pbs
