#include "geometry/continuous_collision_checker.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace pbs {

namespace {

double cross(const Point2D& o, const Point2D& a, const Point2D& b) {
  return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

bool on_segment(const Point2D& p, const Point2D& a, const Point2D& b) {
  return std::min(a.x, b.x) <= p.x + 1e-9 && p.x <= std::max(a.x, b.x) + 1e-9 &&
         std::min(a.y, b.y) <= p.y + 1e-9 && p.y <= std::max(a.y, b.y) + 1e-9;
}

}  // namespace

ContinuousCollisionChecker::ContinuousCollisionChecker(
    std::vector<Polygon> obstacles)
  : obstacles_(std::move(obstacles)) {}

bool ContinuousCollisionChecker::segments_intersect(
    const Point2D& a1, const Point2D& a2,
    const Point2D& b1, const Point2D& b2) {
  double d1 = cross(b1, b2, a1);
  double d2 = cross(b1, b2, a2);
  double d3 = cross(a1, a2, b1);
  double d4 = cross(a1, a2, b2);
  if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
      ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0)))
    return true;
  if (std::abs(d1) < 1e-9 && on_segment(a1, b1, b2)) return true;
  if (std::abs(d2) < 1e-9 && on_segment(a2, b1, b2)) return true;
  if (std::abs(d3) < 1e-9 && on_segment(b1, a1, a2)) return true;
  if (std::abs(d4) < 1e-9 && on_segment(b2, a1, a2)) return true;
  return false;
}

bool ContinuousCollisionChecker::segment_intersects_obstacles(
    const Point2D& a, const Point2D& b) const {
  for (const auto& poly : obstacles_) {
    const auto& v = poly.vertices();
    for (size_t i = 0; i < v.size(); ++i) {
      size_t j = (i + 1) % v.size();
      if (segments_intersect(a, b, v[i], v[j]))
        return true;
    }
  }
  return false;
}

double ContinuousCollisionChecker::point_to_segment_dist(
    const Point2D& p, const Point2D& s1, const Point2D& s2) {
  double dx = s2.x - s1.x, dy = s2.y - s1.y;
  double len2 = dx * dx + dy * dy;
  if (len2 < 1e-18) return std::hypot(p.x - s1.x, p.y - s1.y);
  double t = std::max(0.0, std::min(1.0,
      ((p.x - s1.x) * dx + (p.y - s1.y) * dy) / len2));
  double proj_x = s1.x + t * dx, proj_y = s1.y + t * dy;
  return std::hypot(p.x - proj_x, p.y - proj_y);
}

double ContinuousCollisionChecker::clearance_at(const Point2D& p) const {
  double min_dist = std::numeric_limits<double>::max();
  for (const auto& poly : obstacles_) {
    if (poly.contains(p))
      return 0.0;
    const auto& v = poly.vertices();
    for (size_t i = 0; i < v.size(); ++i) {
      size_t j = (i + 1) % v.size();
      min_dist = std::min(min_dist, point_to_segment_dist(p, v[i], v[j]));
    }
  }
  return (min_dist == std::numeric_limits<double>::max()) ? 1e9 : min_dist;
}

}  // namespace pbs
