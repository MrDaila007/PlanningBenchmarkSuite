#pragma once

#include "point2d.hpp"
#include <vector>

namespace pbs {

class Polygon {
 public:
  Polygon() = default;
  explicit Polygon(std::vector<Point2D> vertices);
  bool contains(const Point2D& p) const;
  void get_bounding_box(double& x_min, double& y_min,
                        double& x_max, double& y_max) const;
  const std::vector<Point2D>& vertices() const { return vertices_; }
  size_t size() const { return vertices_.size(); }

 private:
  std::vector<Point2D> vertices_;
};

}  // namespace pbs
