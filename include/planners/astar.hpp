#pragma once

#include "../core/iplanner.hpp"
#include "../core/path.hpp"
#include "../core/state.hpp"
#include "../environment/ienvironment.hpp"

namespace pbs {

class AStarPlanner : public IPlanner {
 public:
  Path solve(const IEnvironment& env, const State& start,
             const State& goal) override;
  int nodes_expanded() const { return nodes_expanded_; }

 private:
  int nodes_expanded_ = 0;
};

}  // namespace pbs
