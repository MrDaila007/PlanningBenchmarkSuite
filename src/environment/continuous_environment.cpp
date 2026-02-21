#include "environment/continuous_environment.hpp"
#include "geometry/point2d.hpp"
#include <nlohmann/json.hpp>

namespace pbs {

ContinuousEnvironment::ContinuousEnvironment(
    double x_min, double x_max, double y_min, double y_max,
    std::vector<Polygon> obstacles)
  : x_min_(x_min), x_max_(x_max), y_min_(y_min), y_max_(y_max),
    obstacles_(std::move(obstacles)),
    checker_(obstacles_) {}

bool ContinuousEnvironment::is_valid(const State& s) const {
  if (s.x < x_min_ || s.x > x_max_ || s.y < y_min_ || s.y > y_max_)
    return false;
  Point2D p(s.x, s.y);
  for (const auto& poly : obstacles_)
    if (poly.contains(p))
      return false;
  return true;
}

bool ContinuousEnvironment::collision_free(const State& a, const State& b) const {
  Point2D pa(a.x, a.y), pb(b.x, b.y);
  return !checker_.segment_intersects_obstacles(pa, pb);
}

double ContinuousEnvironment::clearance(const State& s) const {
  return checker_.clearance_at(Point2D(s.x, s.y));
}

bool ContinuousEnvironment::get_bounds(double& x_min, double& x_max,
                                       double& y_min, double& y_max) const {
  x_min = x_min_; x_max = x_max_; y_min = y_min_; y_max = y_max_;
  return true;
}

ContinuousEnvironment ContinuousEnvironment::from_json(const std::string& json) {
  auto j = nlohmann::json::parse(json);
  double x_min = j["bounds"]["x_min"];
  double x_max = j["bounds"]["x_max"];
  double y_min = j["bounds"]["y_min"];
  double y_max = j["bounds"]["y_max"];
  std::vector<Polygon> obstacles;
  for (const auto& poly_j : j["obstacles"]) {
    std::vector<Point2D> verts;
    for (const auto& v : poly_j["vertices"])
      verts.emplace_back(v["x"], v["y"]);
    obstacles.emplace_back(std::move(verts));
  }
  return ContinuousEnvironment(x_min, x_max, y_min, y_max, std::move(obstacles));
}

}  // namespace pbs
