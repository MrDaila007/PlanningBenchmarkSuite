#pragma once

#include "metrics.hpp"
#include "../core/path.hpp"
#include "../environment/ienvironment.hpp"

namespace pbs {

class MetricsCollector {
 public:
  Metrics collect(const Path& path, double time_ms, int nodes_expanded,
                  const IEnvironment* env = nullptr) const;
};

}  // namespace pbs
