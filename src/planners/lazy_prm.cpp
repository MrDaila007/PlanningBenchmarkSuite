#include "planners/lazy_prm.hpp"
#include "environment/ienvironment.hpp"
#include "geometry/kdtree2d.hpp"
#include "geometry/point2d.hpp"
#include <random>
#include <algorithm>
#include <queue>
#include <set>
#include <cstddef>
#include <cmath>

namespace pbs {

LazyPRMPlanner::LazyPRMPlanner(int num_samples, int k_neighbors)
  : num_samples_(num_samples), k_neighbors_(k_neighbors) {}

Path LazyPRMPlanner::solve(const IEnvironment& env, const State& start,
                           const State& goal) {
  nodes_expanded_ = 0;

  double x_min = 0, x_max = 0, y_min = 0, y_max = 0;
  if (!env.get_bounds(x_min, x_max, y_min, y_max)) {
    Path p; p.success = false; return p;
  }

  std::vector<Point2D> points;
  points.push_back(Point2D(start.x, start.y));
  points.push_back(Point2D(goal.x, goal.y));

  std::mt19937 rng(42);
  std::uniform_real_distribution<double> ux(x_min, x_max), uy(y_min, y_max);
  int collected = 0, attempts = 0;
  while (collected < num_samples_ && attempts < num_samples_ * 10) {
    double x = ux(rng), y = uy(rng);
    State s(x, y);
    if (env.is_valid(s)) {
      points.push_back(Point2D(x, y));
      collected++;
    }
    attempts++;
  }

  KdTree2D tree;
  tree.build(points);

  std::vector<std::vector<std::pair<size_t, double>>> adj(points.size());
  for (size_t i = 0; i < points.size(); ++i) {
    auto knn = tree.k_nearest(points[i], k_neighbors_ + 1);
    for (size_t j : knn) {
      if (j == i) continue;
      double d = std::hypot(points[j].x - points[i].x, points[j].y - points[i].y);
      adj[i].push_back({j, d});
    }
  }

  std::set<std::pair<size_t, size_t>> invalid_edges;
  auto edge_valid = [&](size_t a, size_t b) {
    if (a > b) std::swap(a, b);
    if (invalid_edges.count({a, b})) return false;
    State sa(points[a].x, points[a].y);
    State sb(points[b].x, points[b].y);
    if (!env.collision_free(sa, sb)) {
      invalid_edges.insert({a, b});
      return false;
    }
    return true;
  };

  const size_t start_idx = 0, goal_idx = 1;
  std::vector<double> dist(points.size(), 1e99);
  std::vector<size_t> parent(points.size(), SIZE_MAX);
  dist[start_idx] = 0;
  using PQ = std::priority_queue<std::pair<double, size_t>,
         std::vector<std::pair<double, size_t>>,
         std::greater<std::pair<double, size_t>>>;
  PQ pq;
  pq.push({0, start_idx});

  while (!pq.empty()) {
    auto [d, u] = pq.top(); pq.pop();
    if (d > dist[u]) continue;
    nodes_expanded_++;
    if (u == goal_idx) break;
    for (const auto& [v, w] : adj[u]) {
      if (!edge_valid(u, v)) continue;
      double nd = dist[u] + w;
      if (nd < dist[v]) {
        dist[v] = nd;
        parent[v] = u;
        pq.push({nd, v});
      }
    }
  }

  Path path;
  if (dist[goal_idx] >= 1e98) {
    path.success = false;
    return path;
  }

  std::vector<size_t> trace;
  for (size_t cur = goal_idx; cur != SIZE_MAX; cur = parent[cur])
    trace.push_back(cur);
  std::reverse(trace.begin(), trace.end());

  for (size_t i : trace)
    path.states.push_back(State(points[i].x, points[i].y));
  path.success = true;
  path.compute_length();
  return path;
}

}  // namespace pbs
