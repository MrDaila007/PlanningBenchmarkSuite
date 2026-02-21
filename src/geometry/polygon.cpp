#include "geometry/polygon.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace pbs {

Polygon::Polygon(std::vector<Point2D> vertices)
  : vertices_(std::move(vertices)) {}

bool Polygon::contains(const Point2D& p) const {
  if (vertices_.size() < 3) return false;
  int n = static_cast<int>(vertices_.size());
  int crossings = 0;
  for (int i = 0; i < n; ++i) {
    int j = (i + 1) % n;
    if (((vertices_[i].y > p.y) != (vertices_[j].y > p.y)) &&
        (p.x < (vertices_[j].x - vertices_[i].x) * (p.y - vertices_[i].y) /
                   (vertices_[j].y - vertices_[i].y) + vertices_[i].x))
      crossings++;
  }
  return (crossings % 2) == 1;
}

void Polygon::get_bounding_box(double& x_min, double& y_min,
                               double& x_max, double& y_max) const {
  x_min = y_min = std::numeric_limits<double>::max();
  x_max = y_max = std::numeric_limits<double>::lowest();
  for (const auto& v : vertices_) {
    x_min = std::min(x_min, v.x);
    y_min = std::min(y_min, v.y);
    x_max = std::max(x_max, v.x);
    y_max = std::max(y_max, v.y);
  }
}

}  // namespace pbs
