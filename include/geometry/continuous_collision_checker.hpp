#pragma once

#include "polygon.hpp"
#include <vector>

namespace pbs {

class ContinuousCollisionChecker {
 public:
  explicit ContinuousCollisionChecker(std::vector<Polygon> obstacles);
  bool segment_intersects_obstacles(const Point2D& a, const Point2D& b) const;
  double clearance_at(const Point2D& p) const;

 private:
  std::vector<Polygon> obstacles_;
  static bool segments_intersect(const Point2D& a1, const Point2D& a2,
                                 const Point2D& b1, const Point2D& b2);
  static double point_to_segment_dist(const Point2D& p,
                                      const Point2D& s1, const Point2D& s2);
};

}  // namespace pbs
