#!/usr/bin/env python3
"""
Export visualization data (environment + path + metrics) for the web UI.

Usage:
  PYTHONPATH=build python3 examples/export_visualization_data.py --config experiments/configs/simple_grid.json --out viz_data.json

Requires: planning_benchmark (pybind11), build with: apt install pybind11-dev && cmake .. && make
"""

import argparse
import json
import sys
from pathlib import Path


def create_planner(name, params):
    """Create planner from config."""
    import planning_benchmark as pbs

    if name == "dijkstra":
        return pbs.DijkstraPlanner()
    if name == "astar":
        return pbs.AStarPlanner()
    if name == "weighted_astar":
        w = params.get("weight", 1.5)
        return pbs.WeightedAStarPlanner(pbs.HeuristicType.Diagonal, w)
    if name == "thetastar":
        return pbs.ThetaStarPlanner()
    if name == "prm":
        n = params.get("num_samples", 500)
        k = params.get("k_neighbors", 10)
        return pbs.PRMPlanner(n, k)
    if name == "lazy_prm":
        n = params.get("num_samples", 500)
        k = params.get("k_neighbors", 10)
        return pbs.LazyPRMPlanner(n, k)
    if name == "rrt":
        step = params.get("step_size", 1.0)
        bias = params.get("goal_bias", 0.1)
        max_i = params.get("max_iter", 5000)
        return pbs.RRTPlanner(step, bias, max_i)
    if name == "rrt_star":
        step = params.get("step_size", 1.0)
        bias = params.get("goal_bias", 0.1)
        max_i = params.get("max_iter", 5000)
        gamma = params.get("rewiring_radius_factor", 10.0)
        return pbs.RRTStarPlanner(step, bias, max_i, gamma)
    if name == "informed_rrt_star":
        step = params.get("step_size", 1.0)
        bias = params.get("goal_bias", 0.1)
        max_i = params.get("max_iter", 5000)
        gamma = params.get("rewiring_radius_factor", 10.0)
        return pbs.InformedRRTStarPlanner(step, bias, max_i, gamma)
    raise ValueError(f"Unknown planner: {name}")


def main():
    parser = argparse.ArgumentParser(description="Export env+path for web visualization")
    parser.add_argument("--config", required=True, help="Path to benchmark config JSON")
    parser.add_argument("--out", default="viz_data.json", help="Output JSON path")
    parser.add_argument("--experiment", type=int, default=0, help="Experiment index (default 0)")
    args = parser.parse_args()

    try:
        import planning_benchmark as pbs
    except ImportError:
        print("Error: planning_benchmark not found. Build with pybind11: apt install pybind11-dev && cmake .. && make")
        print("Then: PYTHONPATH=build python3 examples/export_visualization_data.py --config ...")
        sys.exit(1)

    with open(args.config) as f:
        config = json.load(f)

    if "experiments" not in config or not config["experiments"]:
        print("Error: config must contain 'experiments' array")
        sys.exit(1)

    exp = config["experiments"][args.experiment]
    env_j = exp["environment"]

    if env_j.get("type") != "grid":
        print("Error: only grid environment supported for export")
        sys.exit(1)

    w = env_j["width"]
    h = env_j["height"]
    density = env_j.get("obstacle_density", 0.2)
    seed = env_j.get("seed", 42)
    gen_type = env_j.get("generator", "random_uniform")

    params = pbs.MapGeneratorParams()
    params.width = w
    params.height = h
    params.obstacle_density = density
    params.seed = seed
    params.type = pbs.MapGeneratorType.Maze if gen_type == "maze" else pbs.MapGeneratorType.RandomUniform

    gen = pbs.MapGenerator(seed)
    env = gen.generate(params)

    planner_name = exp.get("planner", "astar")
    planner_params = exp.get("planner_params", {})
    planner = create_planner(planner_name, planner_params)

    start_arr = exp["start"]
    goal_arr = exp["goal"]
    start = pbs.State(start_arr[1], start_arr[0])  # [x,y] -> row,col
    goal = pbs.State(goal_arr[1], goal_arr[0])

    import time
    t0 = time.perf_counter()
    path = planner.solve(env, start, goal)
    t1 = time.perf_counter()
    time_ms = (t1 - t0) * 1000
    nodes = planner.nodes_expanded()

    out = {
        "type": "visualization",
        "environment": json.loads(env.to_json()),
        "start": list(exp["start"]),
        "goal": list(exp["goal"]),
        "path": {
            "states": [{"x": s.x, "y": s.y} for s in path.states],
            "success": path.success,
            "length": path.length,
        },
        "planner": planner_name,
        "time_ms": round(time_ms, 2),
        "nodes_expanded": nodes,
    }

    if planner_name == "informed_rrt_star":
        cd = planner.convergence_data()
        if cd and cd.cost_vs_iteration:
            out["convergence"] = [[int(p[0]), float(p[1])] for p in cd.cost_vs_iteration]

    with open(args.out, "w") as f:
        json.dump(out, f, indent=2)

    print(f"Exported to {args.out} (planner={planner_name}, success={path.success})")


if __name__ == "__main__":
    main()
