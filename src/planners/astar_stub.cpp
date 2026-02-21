#include "planners/astar.hpp"

namespace pbs {

Path AStarPlanner::solve(const IEnvironment& /*env*/, const State& /*start*/,
                        const State& /*goal*/) {
  nodes_expanded_ = 0;
  Path result;
  result.success = false;
  return result;
}

}  // namespace pbs
