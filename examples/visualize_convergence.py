#!/usr/bin/env python3
"""
Example: plot Informed RRT* convergence curve.
Requires: planning_benchmark (pybind11), matplotlib
  pip install matplotlib
  Build with: apt install pybind11-dev && cmake .. && make
  PYTHONPATH=build python3 examples/visualize_convergence.py
"""

try:
    import planning_benchmark as pbs
    HAS_PBS = True
except ImportError:
    HAS_PBS = False

def main():
    if not HAS_PBS:
        print("Install pybind11-dev, build project, set PYTHONPATH=build")
        return

    env = pbs.ContinuousEnvironment(0.0, 10.0, 0.0, 10.0, [])
    planner = pbs.InformedRRTStarPlanner(0.8, 0.1, 2000, 12.0)
    path = planner.solve(env, pbs.State(2, 2), pbs.State(8, 8))

    if not path.success:
        print("No path found")
        return

    cd = planner.convergence_data()
    if not cd.cost_vs_iteration:
        print("No convergence data")
        return

    try:
        import matplotlib.pyplot as plt
        iters = [p[0] for p in cd.cost_vs_iteration]
        costs = [p[1] for p in cd.cost_vs_iteration]
        plt.figure()
        plt.plot(iters, costs)
        plt.xlabel("Iteration")
        plt.ylabel("Best cost")
        plt.title("Informed RRT* Convergence")
        plt.grid(True)
        plt.savefig("convergence.png")
        print("Saved convergence.png")
    except ImportError:
        print("matplotlib not installed; data:", cd.cost_vs_iteration[:5], "...")

if __name__ == "__main__":
    main()
