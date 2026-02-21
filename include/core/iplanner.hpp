#pragma once

#include "path.hpp"
#include "state.hpp"

namespace pbs {

class IEnvironment;

class IPlanner {
 public:
  virtual Path solve(const IEnvironment& env, const State& start,
                    const State& goal) = 0;
  virtual ~IPlanner() = default;
};

}  // namespace pbs
