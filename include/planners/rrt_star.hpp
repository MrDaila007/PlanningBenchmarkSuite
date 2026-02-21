#pragma once

#include "../core/iplanner.hpp"
#include "../core/state.hpp"
#include "../core/path.hpp"

namespace pbs {

class RRTStarPlanner : public IPlanner {
 public:
  RRTStarPlanner(double step_size = 1.0, double goal_bias = 0.1,
                 int max_iter = 5000, double rewiring_radius_factor = 10.0);
  Path solve(const IEnvironment& env, const State& start,
             const State& goal) override;
  int nodes_expanded() const { return nodes_expanded_; }

 private:
  double step_size_;
  double goal_bias_;
  int max_iter_;
  double gamma_;
  mutable int nodes_expanded_ = 0;
};

}  // namespace pbs
