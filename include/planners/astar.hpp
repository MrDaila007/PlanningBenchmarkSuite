#pragma once

#include "../core/iplanner.hpp"
#include "../core/path.hpp"
#include "../core/state.hpp"
#include "../environment/ienvironment.hpp"
#include "heuristic.hpp"

namespace pbs {

class AStarPlanner : public IPlanner {
 public:
  AStarPlanner(HeuristicType h = HeuristicType::Diagonal);
  Path solve(const IEnvironment& env, const State& start,
             const State& goal) override;
  int nodes_expanded() const { return nodes_expanded_; }

 private:
  HeuristicType heuristic_;
  int nodes_expanded_ = 0;
};

}  // namespace pbs
