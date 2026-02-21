#include "environment/se2_environment.hpp"

namespace pbs {

SE2Environment::SE2Environment(std::shared_ptr<ContinuousEnvironment> base)
  : base_(std::move(base)) {}

bool SE2Environment::is_valid(const State& s) const {
  if (!base_->is_valid(s))
    return false;
  if (s.theta && (*s.theta < 0 || *s.theta >= 2 * M_PI))
    return false;
  return true;
}

bool SE2Environment::collision_free(const State& a, const State& b) const {
  return base_->collision_free(a, b);
}

double SE2Environment::clearance(const State& s) const {
  return base_->clearance(s);
}

bool SE2Environment::get_bounds(double& x_min, double& x_max,
                                 double& y_min, double& y_max) const {
  return base_->get_bounds(x_min, x_max, y_min, y_max);
}

}  // namespace pbs
