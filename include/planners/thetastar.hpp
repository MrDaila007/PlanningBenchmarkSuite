#pragma once

#include "astar.hpp"
#include "heuristic.hpp"

namespace pbs {

class ThetaStarPlanner : public IPlanner {
 public:
  ThetaStarPlanner(HeuristicType h = HeuristicType::Diagonal);
  Path solve(const IEnvironment& env, const State& start,
             const State& goal) override;
  int nodes_expanded() const { return nodes_expanded_; }

 private:
  HeuristicType heuristic_;
  int nodes_expanded_ = 0;
};

}  // namespace pbs
