#pragma once

#include "../core/state.hpp"
#include "../environment/ienvironment.hpp"
#include "continuous_environment.hpp"
#include <memory>

namespace pbs {

class SE2Environment : public IEnvironment {
 public:
  explicit SE2Environment(std::shared_ptr<ContinuousEnvironment> base);
  bool is_valid(const State& s) const override;
  bool collision_free(const State& a, const State& b) const override;
  double clearance(const State& s) const override;

 private:
  std::shared_ptr<ContinuousEnvironment> base_;
};

}  // namespace pbs
