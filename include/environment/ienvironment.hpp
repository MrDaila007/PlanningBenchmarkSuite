#pragma once

#include "../core/state.hpp"

namespace pbs {

class IEnvironment {
 public:
  virtual bool is_valid(const State& s) const = 0;
  /// Checks if segment from a to b is obstacle-free.
  /// Grid: Bresenham rasterization; Continuous: segment-polygon intersection.
  virtual bool collision_free(const State& a, const State& b) const = 0;
  virtual double clearance(const State& s) const = 0;
  /// Returns true if environment has bounded workspace (for sampling-based planners).
  virtual bool get_bounds(double& x_min, double& x_max, double& y_min, double& y_max) const {
    (void)x_min; (void)x_max; (void)y_min; (void)y_max;
    return false;
  }
  virtual ~IEnvironment() = default;
};

}  // namespace pbs
