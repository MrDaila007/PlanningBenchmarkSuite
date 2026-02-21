#include <gtest/gtest.h>
#include "benchmark/benchmark_engine.hpp"
#include "benchmark/statistics.hpp"
#include "metrics/metrics_collector.hpp"
#include <cmath>
#include <fstream>

namespace {

TEST(StatisticsTest, MeanStdCI) {
  std::vector<double> v = {1, 2, 3, 4, 5};
  EXPECT_NEAR(pbs::mean(v), 3.0, 1e-9);
  EXPECT_NEAR(pbs::std_dev(v), 1.58113883, 0.01);
  auto [lo, hi] = pbs::confidence_interval_95(v);
  EXPECT_GT(hi, lo);
  EXPECT_LT(lo, 3.0);
  EXPECT_GT(hi, 3.0);
}

TEST(BenchmarkTest, RunFromConfig) {
  const char* config = R"({
    "version": 1,
    "experiments": [{
      "environment": {"type":"grid","width":10,"height":10,
        "generator":"random_uniform","obstacle_density":0.1,"seed":42},
      "planner": "astar",
      "start": [0, 0],
      "goal": [9, 9],
      "repeats": 5
    }]
  })";
  std::ofstream f("/tmp/test_bench_config.json");
  f << config;
  f.close();

  pbs::BenchmarkEngine engine;
  engine.run("/tmp/test_bench_config.json");

  std::ifstream rf("/tmp/test_bench_config_results.json");
  ASSERT_TRUE(rf.good());
  std::string content((std::istreambuf_iterator<char>(rf)),
                      std::istreambuf_iterator<char>());
  rf.close();
  EXPECT_TRUE(content.find("results") != std::string::npos);
  EXPECT_TRUE(content.find("astar") != std::string::npos);
}

}  // namespace
