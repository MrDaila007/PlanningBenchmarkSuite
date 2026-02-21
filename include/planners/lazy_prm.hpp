#pragma once

#include "../core/iplanner.hpp"
#include "../core/state.hpp"
#include "../core/path.hpp"

namespace pbs {

class LazyPRMPlanner : public IPlanner {
 public:
  LazyPRMPlanner(int num_samples = 500, int k_neighbors = 10);
  Path solve(const IEnvironment& env, const State& start,
             const State& goal) override;
  int nodes_expanded() const { return nodes_expanded_; }

 private:
  int num_samples_;
  int k_neighbors_;
  mutable int nodes_expanded_ = 0;
};

}  // namespace pbs
