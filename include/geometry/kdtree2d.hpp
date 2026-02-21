#pragma once

#include "point2d.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <queue>

namespace pbs {

class KdTree2D {
 public:
  KdTree2D() = default;
  void build(const std::vector<Point2D>& points);
  std::vector<size_t> k_nearest(const Point2D& p, size_t k) const;
  std::vector<size_t> radius_search(const Point2D& p, double r) const;
  size_t nearest(const Point2D& p) const;
  size_t size() const { return points_.size(); }
  const Point2D& point(size_t i) const { return points_[i]; }

 private:
  std::vector<Point2D> points_;
  std::vector<size_t> indices_;
  void build_rec(size_t l, size_t r, int axis);
};

}  // namespace pbs
