#pragma once

#include "../core/state.hpp"
#include "../environment/ienvironment.hpp"
#include <string>
#include <vector>

namespace pbs {

class GridEnvironment : public IEnvironment {
 public:
  GridEnvironment(int width, int height);
  GridEnvironment(int width, int height,
                  std::vector<std::vector<int>> occupancy);
  bool is_valid(const State& s) const override;
  bool collision_free(const State& a, const State& b) const override;
  double clearance(const State& s) const override;
  int width() const { return width_; }
  int height() const { return height_; }
  bool occupied(int row, int col) const;
  bool get_bounds(double& x_min, double& x_max, double& y_min, double& y_max) const override;

  static GridEnvironment from_json(const std::string& json);
  std::string to_json() const;

 private:
  int width_ = 0;
  int height_ = 0;
  std::vector<std::vector<int>> occupancy_;
};

}  // namespace pbs
