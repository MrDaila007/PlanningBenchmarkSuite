#pragma once

#include <utility>
#include <vector>

namespace pbs {

struct Metrics {
  double path_length = 0.0;
  double computation_time_ms = 0.0;
  int nodes_expanded = 0;
  bool success = false;
  size_t memory_bytes = 0;

  double smoothness = 0.0;
  double clearance = 0.0;
  double energy = 0.0;

  std::vector<std::pair<int, double>> cost_vs_iteration;
  double gap_to_optimal = 0.0;
};

}  // namespace pbs
