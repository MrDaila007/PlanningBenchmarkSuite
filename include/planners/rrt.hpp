#pragma once

#include "../core/iplanner.hpp"
#include "../core/state.hpp"
#include "../core/path.hpp"

namespace pbs {

class RRTPlanner : public IPlanner {
 public:
  RRTPlanner(double step_size = 1.0, double goal_bias = 0.1, int max_iter = 5000);
  Path solve(const IEnvironment& env, const State& start,
             const State& goal) override;
  int nodes_expanded() const { return nodes_expanded_; }

 private:
  double step_size_;
  double goal_bias_;
  int max_iter_;
  mutable int nodes_expanded_ = 0;
};

}  // namespace pbs
