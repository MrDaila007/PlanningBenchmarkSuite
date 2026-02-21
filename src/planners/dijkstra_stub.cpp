#include "planners/dijkstra.hpp"
#include "environment/ienvironment.hpp"

namespace pbs {

Path DijkstraPlanner::solve(const IEnvironment& /*env*/, const State& /*start*/,
                            const State& /*goal*/) {
  nodes_expanded_ = 0;
  Path result;
  result.success = false;
  return result;
}

}  // namespace pbs
