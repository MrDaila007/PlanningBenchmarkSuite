#include "benchmark/benchmark_engine.hpp"
#include "benchmark/statistics.hpp"
#include "metrics/metrics_collector.hpp"
#include "environment/grid_environment.hpp"
#include "environment/map_generator.hpp"
#include "planners/dijkstra.hpp"
#include "planners/astar.hpp"
#include "planners/weighted_astar.hpp"
#include "planners/thetastar.hpp"
#include "planners/prm.hpp"
#include "planners/lazy_prm.hpp"
#include "planners/rrt.hpp"
#include "planners/rrt_star.hpp"
#include "planners/informed_rrt_star.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <memory>
#include <sstream>

namespace pbs {

namespace {

std::unique_ptr<IPlanner> create_planner(const std::string& name,
                                         const nlohmann::json& params) {
  if (name == "dijkstra") return std::make_unique<DijkstraPlanner>();
  if (name == "astar") return std::make_unique<AStarPlanner>();
  if (name == "weighted_astar") {
    double w = params.value("weight", 1.5);
    return std::make_unique<WeightedAStarPlanner>(HeuristicType::Diagonal, w);
  }
  if (name == "thetastar") return std::make_unique<ThetaStarPlanner>();
  if (name == "prm") {
    int n = params.value("num_samples", 500);
    int k = params.value("k_neighbors", 10);
    return std::make_unique<PRMPlanner>(n, k);
  }
  if (name == "lazy_prm") {
    int n = params.value("num_samples", 500);
    int k = params.value("k_neighbors", 10);
    return std::make_unique<LazyPRMPlanner>(n, k);
  }
  if (name == "rrt") {
    double step = params.value("step_size", 1.0);
    double bias = params.value("goal_bias", 0.1);
    int max_i = params.value("max_iter", 5000);
    return std::make_unique<RRTPlanner>(step, bias, max_i);
  }
  if (name == "rrt_star") {
    double step = params.value("step_size", 1.0);
    double bias = params.value("goal_bias", 0.1);
    int max_i = params.value("max_iter", 5000);
    double gamma = params.value("rewiring_radius_factor", 10.0);
    return std::make_unique<RRTStarPlanner>(step, bias, max_i, gamma);
  }
  if (name == "informed_rrt_star") {
    double step = params.value("step_size", 1.0);
    double bias = params.value("goal_bias", 0.1);
    int max_i = params.value("max_iter", 5000);
    double gamma = params.value("rewiring_radius_factor", 10.0);
    return std::make_unique<InformedRRTStarPlanner>(step, bias, max_i, gamma);
  }
  return nullptr;
}

int get_nodes(const IPlanner* p) {
  if (auto* d = dynamic_cast<const DijkstraPlanner*>(p))
    return d->nodes_expanded();
  if (auto* a = dynamic_cast<const AStarPlanner*>(p))
    return a->nodes_expanded();
  if (auto* w = dynamic_cast<const WeightedAStarPlanner*>(p))
    return w->nodes_expanded();
  if (auto* t = dynamic_cast<const ThetaStarPlanner*>(p))
    return t->nodes_expanded();
  if (auto* prm = dynamic_cast<const PRMPlanner*>(p))
    return prm->nodes_expanded();
  if (auto* lprm = dynamic_cast<const LazyPRMPlanner*>(p))
    return lprm->nodes_expanded();
  if (auto* rrt = dynamic_cast<const RRTPlanner*>(p))
    return rrt->nodes_expanded();
  if (auto* rrtstar = dynamic_cast<const RRTStarPlanner*>(p))
    return rrtstar->nodes_expanded();
  if (auto* irrt = dynamic_cast<const InformedRRTStarPlanner*>(p))
    return irrt->nodes_expanded();
  return 0;
}

}  // namespace

void BenchmarkEngine::run(const std::string& config_path) {
  std::ifstream f(config_path);
  if (!f) {
    std::cerr << "Error: cannot open config " << config_path << "\n";
    return;
  }
  nlohmann::json config;
  try {
    config = nlohmann::json::parse(f);
  } catch (const std::exception& e) {
    std::cerr << "Error: invalid JSON: " << e.what() << "\n";
    return;
  }

  if (!config.contains("experiments") || !config["experiments"].is_array()) {
    std::cerr << "Error: config must contain 'experiments' array\n";
    return;
  }

  MetricsCollector collector;
  nlohmann::json results = nlohmann::json::array();

  for (const auto& exp : config["experiments"]) {
    auto env_j = exp["environment"];
    int w = env_j["width"];
    int h = env_j["height"];
    double density = env_j.value("obstacle_density", 0.2);
    uint64_t seed = env_j.value("seed", 42u);
    MapGeneratorParams mgp{w, h, density, 0, 0.0, seed,
                           MapGeneratorType::RandomUniform};
    MapGenerator gen;
    GridEnvironment env = gen.generate(mgp);

    std::string planner_name = exp.value("planner", "astar");
    auto planner = create_planner(planner_name, exp.value("planner_params", nlohmann::json::object()));
    if (!planner) {
      std::cerr << "Error: unknown planner " << planner_name << "\n";
      continue;
    }

    State start(exp["start"][1].get<int>(), exp["start"][0].get<int>());
    State goal(exp["goal"][1].get<int>(), exp["goal"][0].get<int>());
    int repeats = exp.value("repeats", 30);

    std::vector<double> path_lengths, times, nodes_vec;
    int successes = 0;

    for (int r = 0; r < repeats; ++r) {
      auto t0 = std::chrono::high_resolution_clock::now();
      Path path = planner->solve(env, start, goal);
      auto t1 = std::chrono::high_resolution_clock::now();
      double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

      Metrics m = collector.collect(path, ms, get_nodes(planner.get()), &env);
      if (m.success) successes++;
      path_lengths.push_back(m.path_length);
      times.push_back(ms);
      nodes_vec.push_back(static_cast<double>(m.nodes_expanded));
    }

    auto [ci_pl_l, ci_pl_h] = confidence_interval_95(path_lengths);
    auto [ci_t_l, ci_t_h] = confidence_interval_95(times);
    auto [ci_n_l, ci_n_h] = confidence_interval_95(nodes_vec);

    nlohmann::json res;
    res["planner"] = planner_name;
    res["mean_path_length"] = mean(path_lengths);
    res["std_path_length"] = std_dev(path_lengths);
    res["mean_time_ms"] = mean(times);
    res["std_time_ms"] = std_dev(times);
    res["mean_nodes"] = mean(nodes_vec);
    res["success_rate"] = static_cast<double>(successes) / repeats;
    res["ci_path_length"] = {ci_pl_l, ci_pl_h};
    res["ci_time_ms"] = {ci_t_l, ci_t_h};
    res["repeats"] = repeats;
    results.push_back(res);
  }

  std::string base_path = config_path;
  size_t dot = base_path.rfind('.');
  if (dot != std::string::npos) base_path = base_path.substr(0, dot);

  std::ofstream jf(base_path + "_results.json");
  jf << nlohmann::json{{"results", results}}.dump(2);

  std::ofstream cf(base_path + "_results.csv");
  cf << "planner,mean_path_length,std_path_length,mean_time_ms,std_time_ms,"
     << "mean_nodes,success_rate,ci_low,ci_high\n";
  for (const auto& r : results) {
    cf << r["planner"] << "," << r["mean_path_length"] << ","
       << r["std_path_length"] << "," << r["mean_time_ms"] << ","
       << r["std_time_ms"] << "," << r["mean_nodes"] << ","
       << r["success_rate"] << "," << r["ci_time_ms"][0] << ","
       << r["ci_time_ms"][1] << "\n";
  }
  std::cout << "Results written to " << base_path << "_results.json and .csv\n";
}

}  // namespace pbs
