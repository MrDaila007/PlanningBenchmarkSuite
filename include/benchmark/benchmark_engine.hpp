#pragma once

#include <string>

namespace pbs {

class BenchmarkEngine {
 public:
  void run(const std::string& config_path);
};

}  // namespace pbs
