# Planning Benchmark Suite

C++ research-grade motion planning framework for implementing and comparing planning algorithms (Dijkstra, A*, RRT, RRT*, PRM, Informed RRT*, etc.), parametric environment generation, benchmarking, and statistical analysis.

## Requirements

- C++20 (GCC 10+, Clang 12+)
- CMake 3.20+
- Eigen3, nlohmann-json, GTest

## Build

### Ubuntu/Debian
```bash
sudo apt install build-essential cmake libeigen3-dev nlohmann-json3-dev libgtest-dev
mkdir build && cd build
cmake ..
make -j$(nproc)
ctest --output-on-failure
```

### vcpkg
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake
```

## Usage

```bash
./benchmark --config experiments/configs/simple_grid.json
```

Results: `simple_grid_results.json`, `simple_grid_results.csv`

### Available planners
- **Grid:** dijkstra, astar, weighted_astar, thetastar
- **Sampling:** prm, lazy_prm, rrt, rrt_star, informed_rrt_star

### Example config (experiments/configs/)
- `simple_grid.json` — 20×20 A*, 5 repeats
- `benchmark_suite.json` — multiple planners

## Project structure

| Dir | Contents |
|-----|----------|
| include/ | Headers (core, environment, planners, geometry, metrics, benchmark) |
| src/ | Implementations |
| apps/benchmark/ | CLI executable |
| tests/ | Unit and integration tests |
| experiments/configs/ | JSON configs |
| web/ | Visualization UI (static HTML/JS) |
| docker/ | Dockerfile |
| docs/ | Specification, architecture, REPLICATION |

## Web visualization

Open `web/index.html` in a browser (or serve via `python -m http.server`). Drag-and-drop:
- `*_results.json` / `*_results.csv` — benchmark metrics charts
- `viz_data.json` — map, path, convergence (use `examples/export_visualization_data.py` to generate)

```bash
# Generate viz_data.json (requires pybind11)
PYTHONPATH=build python examples/export_visualization_data.py --config experiments/configs/simple_grid.json --out viz_data.json
```

## Python bindings (optional)

```bash
sudo apt install pybind11-dev
# Rebuild; planning_benchmark.so will be in build/
python3 -c "import planning_benchmark as pbs; env=pbs.GridEnvironment(10,10); print(pbs.AStarPlanner().solve(env, pbs.State(0,0), pbs.State(9,9)).success)"
```

## Docker

```bash
docker build -f docker/Dockerfile -t pbs .
docker run pbs ctest
docker run pbs --config /workspace/experiments/configs/simple_grid.json
```

## Documentation

- [Technical Specification](docs/technical_specification.md)
- [Architecture](docs/architecture.md)
- [Architecture Diagram](docs/architecture_diagram.md)
- [Replication Guide](docs/REPLICATION.md)
- [Development Plan](plan.md)
- [Web Visualization TZ](docs/web_visualization_tz.md) — техническое задание на веб-компонент визуализации

## License

See project license file.
