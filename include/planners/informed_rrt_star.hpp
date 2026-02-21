#pragma once

#include "../core/iplanner.hpp"
#include "../core/state.hpp"
#include "../core/path.hpp"
#include <vector>
#include <utility>

namespace pbs {

struct ConvergenceData {
  std::vector<std::pair<int, double>> cost_vs_iteration;
  double final_cost = 0.0;
  double gap_to_optimal = 0.0;
};

class InformedRRTStarPlanner : public IPlanner {
 public:
  InformedRRTStarPlanner(double step_size = 1.0, double goal_bias = 0.1,
                         int max_iter = 5000, double rewiring_radius_factor = 10.0);
  Path solve(const IEnvironment& env, const State& start,
             const State& goal) override;
  int nodes_expanded() const { return nodes_expanded_; }
  const ConvergenceData& convergence_data() const { return conv_data_; }
  void set_optimal_cost(double c) { optimal_cost_ = c; }

 private:
  double step_size_;
  double goal_bias_;
  int max_iter_;
  double gamma_;
  double optimal_cost_ = -1.0;
  mutable int nodes_expanded_ = 0;
  mutable ConvergenceData conv_data_;
};

}  // namespace pbs
