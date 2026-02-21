#include "planners/dijkstra.hpp"
#include "environment/grid_environment.hpp"
#include <algorithm>
#include <queue>
#include <set>
#include <map>
#include <cmath>

namespace pbs {

namespace {

static int get_row(const State& s) {
  if (s.grid_pos) return s.grid_pos->first;
  return static_cast<int>(s.y);
}
static int get_col(const State& s) {
  if (s.grid_pos) return s.grid_pos->second;
  return static_cast<int>(s.x);
}

const int DR[] = {-1, 1, 0, 0, -1, -1, 1, 1};
const int DC[] = {0, 0, -1, 1, -1, 1, -1, 1};
const double COST[] = {1, 1, 1, 1, M_SQRT2, M_SQRT2, M_SQRT2, M_SQRT2};
const int N_NEIGHBORS = 8;  // 8-connected

}  // namespace

Path DijkstraPlanner::solve(const IEnvironment& env, const State& start,
                            const State& goal) {
  nodes_expanded_ = 0;
  Path result;
  const auto* grid = dynamic_cast<const GridEnvironment*>(&env);
  if (!grid) {
    result.success = false;
    return result;
  }
  int w = grid->width(), h = grid->height();
  int sr = get_row(start), sc = get_col(start);
  int gr = get_row(goal), gc = get_col(goal);

  if (grid->occupied(sr, sc)) {
    result.success = false;
    return result;
  }
  if (grid->occupied(gr, gc)) {
    result.success = false;
    return result;
  }

  using Node = std::pair<double, std::pair<int, int>>;
  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
  pq.push({0.0, {sr, sc}});
  std::map<std::pair<int, int>, std::pair<int, int>> parent;
  std::map<std::pair<int, int>, double> best;
  best[{sr, sc}] = 0.0;

  while (!pq.empty()) {
    auto [g, rc] = pq.top();
    pq.pop();
    int r = rc.first, c = rc.second;
    if (best.count(rc) && best[rc] < g - 1e-9)
      continue;
    nodes_expanded_++;

    if (r == gr && c == gc) {
      result.states.push_back(State(gr, gc));
      auto p = parent.find(rc);
      while (p != parent.end()) {
        result.states.push_back(State(p->second.first, p->second.second));
        p = parent.find(p->second);
      }
      std::reverse(result.states.begin(), result.states.end());
      result.compute_length();
      result.success = true;
      return result;
    }

    for (int i = 0; i < N_NEIGHBORS; ++i) {
      int nr = r + DR[i], nc = c + DC[i];
      if (nr < 0 || nr >= h || nc < 0 || nc >= w)
        continue;
      State from(r, c), to(nr, nc);
      if (grid->occupied(nr, nc) || !env.collision_free(from, to))
        continue;
      double ng = g + COST[i];
      auto nrc = std::make_pair(nr, nc);
      if (best.count(nrc) && best[nrc] <= ng)
        continue;
      best[nrc] = ng;
      parent[nrc] = rc;
      pq.push({ng, nrc});
    }
  }
  result.success = false;
  return result;
}

}  // namespace pbs
