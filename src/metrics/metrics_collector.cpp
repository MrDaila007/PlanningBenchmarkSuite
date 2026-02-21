#include "metrics/metrics_collector.hpp"
#include "core/state.hpp"
#include <cmath>

namespace pbs {

namespace {

double angle_diff(double a1, double a2) {
  double d = a2 - a1;
  while (d > M_PI) d -= 2 * M_PI;
  while (d < -M_PI) d += 2 * M_PI;
  return std::abs(d);
}

double segment_angle(const State& a, const State& b) {
  return std::atan2(b.y - a.y, b.x - a.x);
}

}  // namespace

Metrics MetricsCollector::collect(const Path& path, double time_ms,
                                  int nodes_expanded,
                                  const IEnvironment* env) const {
  Metrics m;
  m.computation_time_ms = time_ms;
  m.nodes_expanded = nodes_expanded;
  m.success = path.success;
  m.path_length = path.length;

  if (path.states.size() < 2) {
    m.smoothness = 0.0;
    m.clearance = env ? env->clearance(path.states.empty() ? State() : path.states[0]) : 0.0;
    m.energy = 0.0;
    return m;
  }

  double smoothness = 0.0;
  for (size_t i = 1; i < path.states.size(); ++i) {
    if (path.states[i].theta && path.states[i-1].theta) {
      smoothness += angle_diff(*path.states[i-1].theta, *path.states[i].theta);
    } else if (i >= 2) {
      double a1 = segment_angle(path.states[i-2], path.states[i-1]);
      double a2 = segment_angle(path.states[i-1], path.states[i]);
      smoothness += angle_diff(a1, a2);
    }
  }
  m.smoothness = smoothness;

  m.clearance = 1e9;
  if (env) {
    for (const auto& s : path.states) {
      double c = env->clearance(s);
      if (c < m.clearance) m.clearance = c;
    }
    if (m.clearance > 1e8) m.clearance = 0.0;
  }

  m.energy = 0.0;
  for (size_t i = 1; i + 1 < path.states.size(); ++i) {
    double d1 = distance(path.states[i-1], path.states[i]);
    double d2 = distance(path.states[i], path.states[i+1]);
    if (d1 < 1e-9 || d2 < 1e-9) continue;
    double a1 = segment_angle(path.states[i-1], path.states[i]);
    double a2 = segment_angle(path.states[i], path.states[i+1]);
    double kappa = angle_diff(a1, a2) / (0.5 * (d1 + d2));
    m.energy += kappa * kappa * 0.5 * (d1 + d2);
  }

  return m;
}

}  // namespace pbs
