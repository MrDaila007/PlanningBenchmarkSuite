#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <chrono>
#include "core/state.hpp"
#include "core/path.hpp"
#include "core/iplanner.hpp"
#include "environment/grid_environment.hpp"
#include "environment/map_generator.hpp"
#include "environment/continuous_environment.hpp"
#include "planners/dijkstra.hpp"
#include "planners/astar.hpp"
#include "planners/weighted_astar.hpp"
#include "planners/thetastar.hpp"
#include "planners/prm.hpp"
#include "planners/lazy_prm.hpp"
#include "planners/rrt.hpp"
#include "planners/rrt_star.hpp"
#include "planners/informed_rrt_star.hpp"
#include "geometry/polygon.hpp"
#include "benchmark/benchmark_engine.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <memory>

namespace py = pybind11;

PYBIND11_MODULE(planning_benchmark, m) {
  m.doc() = "Planning Benchmark Suite - Python bindings";

  py::class_<pbs::State>(m, "State")
    .def(py::init<>())
    .def(py::init<double, double>())
    .def(py::init<double, double, double>())
    .def_readwrite("x", &pbs::State::x)
    .def_readwrite("y", &pbs::State::y);

  py::class_<pbs::Path>(m, "Path")
    .def_readwrite("states", &pbs::Path::states)
    .def_readwrite("success", &pbs::Path::success)
    .def_readwrite("length", &pbs::Path::length)
    .def("empty", &pbs::Path::empty);

  py::class_<pbs::IEnvironment>(m, "IEnvironment");

  py::class_<pbs::GridEnvironment, pbs::IEnvironment>(m, "GridEnvironment")
    .def(py::init<int, int>())
    .def("width", &pbs::GridEnvironment::width)
    .def("height", &pbs::GridEnvironment::height)
    .def("to_json", &pbs::GridEnvironment::to_json)
    .def_static("from_json", [](const std::string& s) {
      return pbs::GridEnvironment::from_json(s);
    });

  py::class_<pbs::ContinuousEnvironment, pbs::IEnvironment>(m, "ContinuousEnvironment")
    .def(py::init<double, double, double, double, std::vector<pbs::Polygon>>())
    .def_static("from_json", [](const std::string& s) {
      return pbs::ContinuousEnvironment::from_json(s);
    });

  py::enum_<pbs::MapGeneratorType>(m, "MapGeneratorType")
    .value("RandomUniform", pbs::MapGeneratorType::RandomUniform)
    .value("Maze", pbs::MapGeneratorType::Maze);

  py::class_<pbs::MapGeneratorParams>(m, "MapGeneratorParams")
    .def(py::init<>())
    .def_readwrite("width", &pbs::MapGeneratorParams::width)
    .def_readwrite("height", &pbs::MapGeneratorParams::height)
    .def_readwrite("obstacle_density", &pbs::MapGeneratorParams::obstacle_density)
    .def_readwrite("seed", &pbs::MapGeneratorParams::seed)
    .def_readwrite("type", &pbs::MapGeneratorParams::type);

  py::class_<pbs::MapGenerator>(m, "MapGenerator")
    .def(py::init<>())
    .def(py::init<uint64_t>(), py::arg("seed"))
    .def("generate", &pbs::MapGenerator::generate);

  py::enum_<pbs::HeuristicType>(m, "HeuristicType")
    .value("Manhattan", pbs::HeuristicType::Manhattan)
    .value("Euclidean", pbs::HeuristicType::Euclidean)
    .value("Diagonal", pbs::HeuristicType::Diagonal);

  // Base class must be registered before derived classes
  py::class_<pbs::IPlanner>(m, "IPlanner");

  py::class_<pbs::DijkstraPlanner, pbs::IPlanner>(m, "DijkstraPlanner")
    .def(py::init<>())
    .def("solve", &pbs::DijkstraPlanner::solve)
    .def("nodes_expanded", &pbs::DijkstraPlanner::nodes_expanded);

  py::class_<pbs::AStarPlanner, pbs::IPlanner>(m, "AStarPlanner")
    .def(py::init<>())
    .def("solve", &pbs::AStarPlanner::solve)
    .def("nodes_expanded", &pbs::AStarPlanner::nodes_expanded);

  py::class_<pbs::WeightedAStarPlanner, pbs::IPlanner>(m, "WeightedAStarPlanner")
    .def(py::init<pbs::HeuristicType, double>(),
         py::arg("heuristic") = pbs::HeuristicType::Diagonal,
         py::arg("weight") = 1.5)
    .def("solve", &pbs::WeightedAStarPlanner::solve)
    .def("nodes_expanded", &pbs::WeightedAStarPlanner::nodes_expanded);

  py::class_<pbs::ThetaStarPlanner, pbs::IPlanner>(m, "ThetaStarPlanner")
    .def(py::init<>())
    .def("solve", &pbs::ThetaStarPlanner::solve)
    .def("nodes_expanded", &pbs::ThetaStarPlanner::nodes_expanded);

  py::class_<pbs::PRMPlanner, pbs::IPlanner>(m, "PRMPlanner")
    .def(py::init<int, int>(), py::arg("num_samples") = 500, py::arg("k_neighbors") = 10)
    .def("solve", &pbs::PRMPlanner::solve)
    .def("nodes_expanded", &pbs::PRMPlanner::nodes_expanded);

  py::class_<pbs::LazyPRMPlanner, pbs::IPlanner>(m, "LazyPRMPlanner")
    .def(py::init<int, int>(), py::arg("num_samples") = 500, py::arg("k_neighbors") = 10)
    .def("solve", &pbs::LazyPRMPlanner::solve)
    .def("nodes_expanded", &pbs::LazyPRMPlanner::nodes_expanded);

  py::class_<pbs::RRTPlanner, pbs::IPlanner>(m, "RRTPlanner")
    .def(py::init<double, double, int>(),
         py::arg("step_size") = 1.0, py::arg("goal_bias") = 0.1, py::arg("max_iter") = 5000)
    .def("solve", &pbs::RRTPlanner::solve)
    .def("nodes_expanded", &pbs::RRTPlanner::nodes_expanded);

  py::class_<pbs::RRTStarPlanner, pbs::IPlanner>(m, "RRTStarPlanner")
    .def(py::init<double, double, int, double>(),
         py::arg("step_size") = 1.0, py::arg("goal_bias") = 0.1,
         py::arg("max_iter") = 5000, py::arg("gamma") = 10.0)
    .def("solve", &pbs::RRTStarPlanner::solve)
    .def("nodes_expanded", &pbs::RRTStarPlanner::nodes_expanded);

  py::class_<pbs::ConvergenceData>(m, "ConvergenceData")
    .def_readonly("cost_vs_iteration", &pbs::ConvergenceData::cost_vs_iteration)
    .def_readonly("final_cost", &pbs::ConvergenceData::final_cost)
    .def_readonly("gap_to_optimal", &pbs::ConvergenceData::gap_to_optimal);

  py::class_<pbs::InformedRRTStarPlanner, pbs::IPlanner>(m, "InformedRRTStarPlanner")
    .def(py::init<double, double, int, double>(),
         py::arg("step_size") = 1.0, py::arg("goal_bias") = 0.1,
         py::arg("max_iter") = 5000, py::arg("gamma") = 10.0)
    .def("solve", &pbs::InformedRRTStarPlanner::solve)
    .def("nodes_expanded", &pbs::InformedRRTStarPlanner::nodes_expanded)
    .def("convergence_data", &pbs::InformedRRTStarPlanner::convergence_data,
         py::return_value_policy::reference_internal);

  py::class_<pbs::Point2D>(m, "Point2D")
    .def(py::init<double, double>())
    .def_readwrite("x", &pbs::Point2D::x)
    .def_readwrite("y", &pbs::Point2D::y);

  py::class_<pbs::Polygon>(m, "Polygon")
    .def(py::init<std::vector<pbs::Point2D>>());

  m.def("run_benchmark", [](const std::string& config_path) {
    pbs::BenchmarkEngine engine;
    engine.run(config_path);
    std::string out_path = config_path;
    size_t dot = out_path.rfind('.');
    if (dot != std::string::npos) out_path = out_path.substr(0, dot);
    std::ifstream f(out_path + "_results.json");
    std::vector<py::dict> results;
    if (f) {
      auto j = nlohmann::json::parse(f);
      if (j.contains("results"))
        for (const auto& r : j["results"]) {
          py::dict d;
          d["planner"] = r.value("planner", std::string());
          d["mean_path_length"] = r.value("mean_path_length", 0.0);
          d["std_path_length"] = r.value("std_path_length", 0.0);
          d["mean_time_ms"] = r.value("mean_time_ms", 0.0);
          d["mean_nodes"] = r.value("mean_nodes", 0.0);
          d["success_rate"] = r.value("success_rate", 0.0);
          results.push_back(d);
        }
    }
    return results;
  });

  m.def("run_experiment", [](const pbs::GridEnvironment& env,
                             pbs::IPlanner& planner,
                             const pbs::State& start, const pbs::State& goal,
                             int repeats) {
    py::dict metrics;
    int successes = 0;
    double total_len = 0, total_time = 0;
    for (int r = 0; r < repeats; ++r) {
      auto t0 = std::chrono::high_resolution_clock::now();
      pbs::Path path = planner.solve(env, start, goal);
      auto t1 = std::chrono::high_resolution_clock::now();
      double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
      if (path.success) { successes++; total_len += path.length; }
      total_time += ms;
    }
    metrics["success_rate"] = static_cast<double>(successes) / repeats;
    metrics["mean_path_length"] = successes > 0 ? total_len / successes : 0.0;
    metrics["mean_time_ms"] = total_time / repeats;
    return metrics;
  }, py::arg("env"), py::arg("planner"), py::arg("start"), py::arg("goal"),
     py::arg("repeats") = 10);
}
