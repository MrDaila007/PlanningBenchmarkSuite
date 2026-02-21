# Planning Benchmark Suite

C++ research-grade motion planning framework for implementing and comparing planning algorithms (Dijkstra, A*, RRT, RRT*, etc.), parametric environment generation, benchmarking, and statistical analysis.

## Build

### Prerequisites
- C++20 compiler (GCC 10+, Clang 12+)
- CMake 3.20+
- Dependencies: Eigen3, nlohmann-json, GTest

### Ubuntu/Debian
```bash
sudo apt install build-essential cmake libeigen3-dev nlohmann-json3-dev libgtest-dev
mkdir build && cd build
cmake ..
make
ctest
```

### With vcpkg
```bash
# Install vcpkg, then:
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
make
ctest
```

## Project structure
- `include/` — headers (core, environment, planners, geometry, metrics, benchmark)
- `src/` — implementations
- `apps/benchmark/` — CLI benchmark
- `tests/` — unit tests
- `docs/` — technical spec, architecture, TZ
- `plan.md` — development plan
- `tasks/` — task descriptions

## Documentation
- [Technical Specification](docs/technical_specification.md)
- [Architecture](docs/architecture.md)
- [Development Plan](plan.md)
