#include "planners/astar.hpp"
#include "planners/heuristic.hpp"
#include "environment/grid_environment.hpp"
#include <algorithm>
#include <queue>
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
const int N_NEIGHBORS = 8;

}  // namespace

AStarPlanner::AStarPlanner(HeuristicType h) : heuristic_(h) {}

Path AStarPlanner::solve(const IEnvironment& env, const State& start,
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

  if (grid->occupied(sr, sc) || grid->occupied(gr, gc)) {
    result.success = false;
    return result;
  }

  auto h_func = [this, gr, gc](int r, int c) {
    switch (heuristic_) {
      case HeuristicType::Manhattan:
        return heuristic_manhattan(r, c, gr, gc);
      case HeuristicType::Euclidean:
        return heuristic_euclidean(r, c, gr, gc);
      case HeuristicType::Diagonal:
        return heuristic_diagonal(r, c, gr, gc);
      default:
        return heuristic_manhattan(r, c, gr, gc);
    }
  };

  using Node = std::pair<double, std::pair<int, int>>;
  std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
  double f0 = h_func(sr, sc);
  pq.push({f0, {sr, sc}});
  std::map<std::pair<int, int>, std::pair<int, int>> parent;
  std::map<std::pair<int, int>, double> g_best;
  g_best[{sr, sc}] = 0.0;

  while (!pq.empty()) {
    auto [f, rc] = pq.top();
    pq.pop();
    int r = rc.first, c = rc.second;
    double g = g_best[rc];
    if (g + 1e-9 < f - h_func(r, c))
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
      if (g_best.count(nrc) && g_best[nrc] <= ng)
        continue;
      g_best[nrc] = ng;
      parent[nrc] = rc;
      double nf = ng + h_func(nr, nc);
      pq.push({nf, nrc});
    }
  }
  result.success = false;
  return result;
}

}  // namespace pbs
