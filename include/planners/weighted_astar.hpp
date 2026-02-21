#pragma once

#include "astar.hpp"
#include "heuristic.hpp"

namespace pbs {

class WeightedAStarPlanner : public IPlanner {
 public:
  WeightedAStarPlanner(HeuristicType h = HeuristicType::Diagonal,
                      double w = 1.5);
  Path solve(const IEnvironment& env, const State& start,
             const State& goal) override;
  int nodes_expanded() const { return nodes_expanded_; }

 private:
  HeuristicType heuristic_;
  double weight_;
  int nodes_expanded_ = 0;
};

}  // namespace pbs
