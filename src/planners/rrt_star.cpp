#include "planners/rrt_star.hpp"
#include "environment/ienvironment.hpp"
#include "geometry/kdtree2d.hpp"
#include "geometry/point2d.hpp"
#include <random>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstddef>

namespace pbs {

namespace {

double rrt_star_radius(size_t n, double gamma, int dim, double step_size) {
  if (n <= 1) return 1e9;
  double r = gamma * std::pow(std::log(static_cast<double>(n)) / n, 1.0 / dim);
  return std::min(r, step_size * 2.0);
}

}  // namespace

RRTStarPlanner::RRTStarPlanner(double step_size, double goal_bias,
                               int max_iter, double rewiring_radius_factor)
  : step_size_(step_size), goal_bias_(goal_bias),
    max_iter_(max_iter), gamma_(rewiring_radius_factor) {}

Path RRTStarPlanner::solve(const IEnvironment& env, const State& start,
                          const State& goal) {
  nodes_expanded_ = 0;

  double x_min = 0, x_max = 0, y_min = 0, y_max = 0;
  if (!env.get_bounds(x_min, x_max, y_min, y_max)) {
    Path p; p.success = false; return p;
  }

  std::vector<Point2D> tree;
  tree.push_back(Point2D(start.x, start.y));
  std::vector<size_t> parent;
  parent.push_back(0);
  std::vector<double> cost;
  cost.push_back(0.0);

  std::mt19937 rng(42);
  std::uniform_real_distribution<double> u01(0, 1), ux(x_min, x_max), uy(y_min, y_max);

  const double goal_thresh = step_size_ * 1.5;
  double best_cost = 1e99;
  size_t best_goal_idx = SIZE_MAX;

  for (int iter = 0; iter < max_iter_; ++iter) {
    Point2D sample;
    if (u01(rng) < goal_bias_) {
      sample.x = goal.x; sample.y = goal.y;
    } else {
      sample.x = ux(rng); sample.y = uy(rng);
    }

    size_t near_idx = 0;
    double near_d2 = 1e99;
    for (size_t i = 0; i < tree.size(); ++i) {
      double dx = sample.x - tree[i].x, dy = sample.y - tree[i].y;
      double d2 = dx * dx + dy * dy;
      if (d2 < near_d2) { near_d2 = d2; near_idx = i; }
    }

    double dx = sample.x - tree[near_idx].x, dy = sample.y - tree[near_idx].y;
    double d = std::sqrt(dx * dx + dy * dy);
    Point2D new_pt;
    if (d <= step_size_ || d < 1e-9) {
      new_pt = sample;
    } else {
      new_pt.x = tree[near_idx].x + step_size_ * dx / d;
      new_pt.y = tree[near_idx].y + step_size_ * dy / d;
    }

    State a(tree[near_idx].x, tree[near_idx].y);
    State b(new_pt.x, new_pt.y);
    if (!env.collision_free(a, b)) continue;
    if (!env.is_valid(State(new_pt.x, new_pt.y))) continue;

    double seg_cost = std::hypot(new_pt.x - tree[near_idx].x, new_pt.y - tree[near_idx].y);
    double c_min = cost[near_idx] + seg_cost;
    size_t best_parent = near_idx;

    double r = rrt_star_radius(tree.size(), gamma_, 2, step_size_);
    for (size_t i = 0; i < tree.size(); ++i) {
      double dist = std::hypot(tree[i].x - new_pt.x, tree[i].y - new_pt.y);
      if (dist > r) continue;
      State ai(tree[i].x, tree[i].y);
      if (!env.collision_free(ai, b)) continue;
      double c = cost[i] + dist;
      if (c < c_min) {
        c_min = c;
        best_parent = i;
      }
    }

    tree.push_back(new_pt);
    parent.push_back(best_parent);
    cost.push_back(c_min);

    for (size_t i = 0; i < tree.size() - 1; ++i) {
      double dist = std::hypot(tree[i].x - new_pt.x, tree[i].y - new_pt.y);
      if (dist > r) continue;
      State ai(tree[i].x, tree[i].y);
      if (!env.collision_free(ai, b)) continue;
      double c_new = cost.back() + dist;
      if (c_new < cost[i]) {
        parent[i] = tree.size() - 1;
        cost[i] = c_new;
        std::vector<size_t> stack = {i};
        while (!stack.empty()) {
          size_t u = stack.back(); stack.pop_back();
          for (size_t j = 0; j < tree.size() - 1; ++j) {
            if (parent[j] == u) {
              double d = std::hypot(tree[j].x - tree[u].x, tree[j].y - tree[u].y);
              cost[j] = cost[u] + d;
              stack.push_back(j);
            }
          }
        }
      }
    }

    nodes_expanded_ = static_cast<int>(tree.size());

    double to_goal = std::hypot(goal.x - new_pt.x, goal.y - new_pt.y);
    if (to_goal < goal_thresh) {
      State sa(new_pt.x, new_pt.y), sg(goal.x, goal.y);
      if (env.collision_free(sa, sg)) {
        double c_goal = cost.back() + to_goal;
        if (c_goal < best_cost) {
          best_cost = c_goal;
          best_goal_idx = tree.size() - 1;
        }
      }
    }
  }

  if (best_goal_idx == SIZE_MAX) {
    Path path;
    path.success = false;
    return path;
  }

  Path path;
  std::vector<size_t> trace;
  for (size_t cur = best_goal_idx; ; cur = parent[cur]) {
    trace.push_back(cur);
    if (cur == 0) break;
  }
  std::reverse(trace.begin(), trace.end());
  for (size_t i : trace)
    path.states.push_back(State(tree[i].x, tree[i].y));
  path.states.push_back(State(goal.x, goal.y));
  path.success = true;
  path.compute_length();
  return path;
}

}  // namespace pbs
