#include "planners/thetastar.hpp"
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

inline double dist(int r1, int c1, int r2, int c2) {
  int dr = r2 - r1, dc = c2 - c1;
  return std::sqrt(static_cast<double>(dr * dr + dc * dc));
}

const int DR[] = {-1, 1, 0, 0, -1, -1, 1, 1};
const int DC[] = {0, 0, -1, 1, -1, 1, -1, 1};
const int N_NEIGHBORS = 8;

}  // namespace

ThetaStarPlanner::ThetaStarPlanner(HeuristicType h) : heuristic_(h) {}

Path ThetaStarPlanner::solve(const IEnvironment& env, const State& start,
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
  pq.push({h_func(sr, sc), {sr, sc}});
  std::map<std::pair<int, int>, std::pair<int, int>> parent;
  std::map<std::pair<int, int>, double> g_best;
  parent[{sr, sc}] = {-1, -1};
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
      auto rc_cur = rc;
      while (parent[rc_cur].first >= 0) {
        rc_cur = parent[rc_cur];
        result.states.push_back(State(rc_cur.first, rc_cur.second));
      }
      std::reverse(result.states.begin(), result.states.end());
      result.compute_length();
      result.success = true;
      return result;
    }

    auto pr = parent[rc];
    bool has_parent = (pr.first >= 0 && pr.second >= 0);

    for (int i = 0; i < N_NEIGHBORS; ++i) {
      int nr = r + DR[i], nc = c + DC[i];
      if (nr < 0 || nr >= h || nc < 0 || nc >= w)
        continue;
      if (grid->occupied(nr, nc))
        continue;
      auto nrc = std::make_pair(nr, nc);

      double ng;
      std::pair<int, int> new_parent;
      if (has_parent) {
        State from_parent(pr.first, pr.second), to_n(nr, nc);
        if (env.collision_free(from_parent, to_n)) {
          double g_parent = g_best[pr];
          ng = g_parent + dist(pr.first, pr.second, nr, nc);
          new_parent = pr;
        } else {
          State from_cur(r, c), to_n(nr, nc);
          if (!env.collision_free(from_cur, to_n))
            continue;
          ng = g + dist(r, c, nr, nc);
          new_parent = rc;
        }
      } else {
        State from_cur(r, c), to_n(nr, nc);
        if (!env.collision_free(from_cur, to_n))
          continue;
        ng = g + dist(r, c, nr, nc);
        new_parent = rc;
      }

      if (g_best.count(nrc) && g_best[nrc] <= ng)
        continue;
      g_best[nrc] = ng;
      parent[nrc] = new_parent;
      pq.push({ng + h_func(nr, nc), nrc});
    }
  }
  result.success = false;
  return result;
}

}  // namespace pbs
