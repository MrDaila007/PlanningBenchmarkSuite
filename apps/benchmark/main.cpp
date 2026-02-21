#include "benchmark/benchmark_engine.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << (argc > 0 ? argv[0] : "benchmark")
              << " --config <config.json>\n";
    return 1;
  }
  std::string config_path;
  for (int i = 1; i < argc - 1; ++i) {
    if (std::string(argv[i]) == "--config") {
      config_path = argv[i + 1];
      break;
    }
  }
  if (config_path.empty()) {
    std::cerr << "Error: --config required\n";
    return 1;
  }
  pbs::BenchmarkEngine engine;
  engine.run(config_path);
  return 0;
}
