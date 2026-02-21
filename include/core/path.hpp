#pragma once

#include "state.hpp"
#include <vector>
#include <cmath>

namespace pbs {

struct Path {
  std::vector<State> states;
  bool success = false;
  double length = 0.0;

  bool empty() const { return states.empty(); }

  void compute_length() {
    length = 0.0;
    for (size_t i = 1; i < states.size(); ++i) {
      length += distance(states[i - 1], states[i]);
    }
  }
};

}  // namespace pbs
