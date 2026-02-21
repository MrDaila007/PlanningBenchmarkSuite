#pragma once

#include "../environment/ienvironment.hpp"
#include <vector>

namespace pbs {

class GridEnvironment : public IEnvironment {
 public:
  GridEnvironment(int width, int height);
  bool is_valid(const State& s) const override;
  bool collision_free(const State& a, const State& b) const override;
  double clearance(const State& s) const override;
  int width() const { return width_; }
  int height() const { return height_; }
  bool occupied(int row, int col) const;

 private:
  int width_ = 0;
  int height_ = 0;
  std::vector<std::vector<int>> occupancy_;
};

}  // namespace pbs
