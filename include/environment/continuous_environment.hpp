#pragma once

#include "../core/state.hpp"
#include "../environment/ienvironment.hpp"
#include "../geometry/continuous_collision_checker.hpp"
#include "../geometry/polygon.hpp"
#include <string>
#include <vector>

namespace pbs {

class ContinuousEnvironment : public IEnvironment {
 public:
  ContinuousEnvironment(double x_min, double x_max, double y_min, double y_max,
                        std::vector<Polygon> obstacles);
  bool is_valid(const State& s) const override;
  bool collision_free(const State& a, const State& b) const override;
  double clearance(const State& s) const override;
  static ContinuousEnvironment from_json(const std::string& json);
  bool get_bounds(double& x_min, double& x_max, double& y_min, double& y_max) const override;

 private:
  double x_min_ = 0, x_max_ = 0, y_min_ = 0, y_max_ = 0;
  std::vector<Polygon> obstacles_;
  ContinuousCollisionChecker checker_;
};

}  // namespace pbs
