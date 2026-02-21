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
  virtual ~IEnvironment() = default;
};

}  // namespace pbs
