# Autonomous Web Plan — Planning Benchmark Suite

**Version:** 1.0  
**Date:** 2025-02-21  
**Purpose:** Technical specification and implementation plan for making the web visualization fully autonomous (no Python/C++ server dependence).

---

# 1. Executive Summary

## Current State

| Component | Implementation | Dependencies |
|-----------|----------------|--------------|
| Web UI | `web/` — HTML/CSS/JS | Chart.js (CDN) |
| Map display | Canvas, `MapRenderer` (inlined in index.html) | — |
| Charts | `js/charts.js` | Chart.js |
| Data input | Drag-and-drop JSON/CSV | — |
| Map generation | C++ `MapGenerator` → Python export → JSON | Python, C++ build |
| Planning | C++ planners → Python bindings → export | Python, C++ build |
| Benchmark | C++ `BenchmarkEngine` | CLI, config files |

**Data flow today:** User runs `./benchmark --config X.json` or `python export_visualization_data.py --config X.json` → loads JSON/CSV in browser → visualization.

## Goal — Fully Autonomous Web

1. **Generate map in browser** — User selects params (size, type: random/maze), clicks Generate → map appears, no backend.
2. **Run planner in browser** — User picks algorithm (Dijkstra, A*, RRT*, etc.), clicks Solve → path and metrics appear, optional step-by-step visualization.
3. **Run benchmark in browser** — User clicks "Run benchmark" → all available algorithms run, metrics collected, comparison table/charts.
4. **No dependence on Python/C++ server** — Works offline after initial load.

---

# 2. Technology Options

## Option A: Pure JavaScript Ports

**Approach:** Port C++ algorithms to JavaScript (vanilla JS or ES modules). Implement map generators (random_uniform, maze) and planners (Dijkstra, A*, RRT*, etc.) in JS.

| Pros | Cons |
|------|------|
| No build step for planning logic | Duplication of logic (C++ + JS) |
| Easy to debug, modify, experiment | Potential divergence from C++ behavior |
| Full control over step-by-step visualization | Performance may be slower for large maps |
| Works in all modern browsers | RRT/RRT*: more complex (sampling, rewiring) |
| No WASM toolchain | |

## Option B: Emscripten / WebAssembly

**Approach:** Compile C++ library (MapGenerator, planners, GridEnvironment) to WASM via Emscripten. Call from JS.

| Pros | Cons |
|------|------|
| Single source of truth (C++ code) | Requires Emscripten toolchain, CMake changes |
| Exact parity with CLI/benchmark | Step-by-step visualization harder (callbacks/iterators) |
| Performance close to native for heavy workloads | Debugging WASM more complex |
| PRM, RRT*, etc. benefit from C++ speed | Build complexity, larger initial load |

## Recommendation

| Phase | Technology | Rationale |
|-------|-------------|-----------|
| MVP | **Pure JS** for map gen + Dijkstra + A* | Fastest path to demo; no build changes; easy step-by-step |
| Phase 2 | **Pure JS** for RRT, RRT* | Keep MVP approach; algorithms are well-documented, ~500–1000 lines each |
| Phase 3 (optional) | **Emscripten** for full suite | If perf critical (e.g., 500×500 mazes, Informed RRT* 50k iters); can coexist with JS versions |

**Hybrid:** Use Pure JS for MVP and Phase 2. Evaluate Emscripten only if users report perf issues or need parity with C++ on large instances.

---

# 3. User Flows

## 3.1 Map Generation Flow

```
User → Sidebar: "Generate Map"
  → Modal/panel: width, height, type (random_uniform | maze), seed, obstacle_density (random only)
  → [Generate] → JS MapGenerator runs
  → occupancy grid produced
  → MapRenderer displays map; start=(0,0), goal=(width-1,height-1) or configurable
  → "Solve" and "Run benchmark" become enabled
```

**Configurable start/goal:** Allow user to click on map (or enter coords) to set start/goal.

## 3.2 Single Planner Flow

```
User → Map displayed (generated or loaded)
  → Sidebar: Select planner (Dijkstra, A*, RRT*, …)
  → [Solve] → Planner runs in JS (sync or async with progress)
  → Path + metrics displayed
  → Optional: [Step-by-step] shows path construction (for A*: expansion order; for RRT*: tree growth)
```

**Optional step-by-step:**
- A*: callback per expansion → animate node coloring
- RRT*: callback per iteration → draw tree edges, path when found

## 3.3 Benchmark Mode Flow

```
User → Map displayed
  → [Run benchmark] → All enabled planners run sequentially (or parallel via Web Workers)
  → Per planner: solve N times (e.g., N=5 for MVP), collect path_length, time_ms, nodes
  → Aggregate: mean, std, success_rate
  → Display: comparison table + bar charts (reuse existing Charts)
  → Optional: export results as JSON/CSV (download)
```

**Concurrency:** Use Web Workers to run planners in parallel (one worker per planner) to avoid blocking UI. Fallback: run sequentially with `requestAnimationFrame` or `setTimeout` between planners.

---

# 4. Data Structures & APIs

## 4.1 Environment (Grid)

```javascript
// Matches C++ GridEnvironment / Python export
const environment = {
  type: 'grid',
  width: 20,
  height: 20,
  occupancy: [  // row-major: occupancy[row][col], 0=free, 1=obstacle
    [0, 0, 1, ...],
    [0, 0, 0, ...],
    ...
  ]
};
```

## 4.2 State & Path

```javascript
const state = { x: 0, y: 0 };  // or { x: col, y: row } for grid (col, row)
const path = {
  states: [{ x: 0, y: 0 }, { x: 1, y: 0 }, ...],
  success: true,
  length: 25.5
};
```

**Coordinate convention:** Same as C++ — `(x, y)` where for grid `x` = column, `y` = row. `start: [0, 0]`, `goal: [width-1, height-1]`.

## 4.3 MapGenerator API (JS)

```javascript
// js/map-generator.js
const MapGenerator = {
  generate(params) {
    const { width, height, type, obstacle_density, seed } = params;
    if (type === 'random_uniform') return this.randomUniform(width, height, obstacle_density, seed);
    if (type === 'maze') return this.maze(width, height, seed);  // width/height = cells
    throw new Error(`Unknown type: ${type}`);
  },
  randomUniform(width, height, density, seed) { /* port of C++ */ },
  maze(cellsWide, cellsHigh, seed) { /* Kruskal MST */ }
};
```

**Maze convention:** For `maze(W, H)` with W×H cells, occupancy grid is `(2*H+1) × (2*W+1)`. Start/goal typically `[1,1]` and `[2*H-1, 2*W-1]` in grid coords.

## 4.4 Planner API (JS)

```javascript
// js/planners/planner-interface.js
// Each planner implements:
function solve(environment, start, goal, options = {}) {
  return {
    path: { states, success, length },
    nodes_expanded: number,
    // optional:
    convergence: [[iteration, cost], ...],  // RRT*
    onStep: (callback) => void  // for step-by-step; planner calls callback each step
  };
}
```

**Options:** `step_size`, `goal_bias`, `max_iter`, etc. for RRT-family; `weight` for weighted A*.

## 4.5 Benchmark API (JS)

```javascript
// js/benchmark-runner.js
function runBenchmark(environment, start, goal, plannerNames, repeats = 5) {
  const results = [];
  for (const name of plannerNames) {
    const runs = [];
    for (let r = 0; r < repeats; r++) {
      const { path, nodes_expanded } = planners[name].solve(env, start, goal);
      runs.push({ path_length: path.length, time_ms, nodes_expanded, success: path.success });
    }
    results.push(aggregate(name, runs));  // mean, std, success_rate
  }
  return { results };
}
```

---

# 5. UI Layout & Integration

## 5.1 Proposed Layout (extends current web/index.html)

```
┌─────────────────────────────────────────────────────────────────────────┐
│ Planning Benchmark Suite — Autonomous Web                               │
├──────────────┬──────────────────────────────────────────────────────────┤
│ Sidebar      │ Main content                                             │
│              │                                                          │
│ [Examples]   │ ┌─ Map & Path ─────────────────────────────────────┐   │
│ (dropdown)   │ │  [Canvas]                                        │   │
│              │ │  Start: ●  Goal: ●  Path: ─────                    │   │
│ Or load      │ └───────────────────────────────────────────────────┘   │
│ (drop zone)  │                                                          │
│              │ ┌─ Generate Map ────────────────────────────────────┐   │
│ NEW:         │ │  W: [20] H: [20] Type: [random|maze] Seed: [42]  │   │
│ [Generate]   │ │  Density: [0.1] (random only)  [Generate]        │   │
│              │ └───────────────────────────────────────────────────┘   │
│ NEW:         │                                                          │
│ Planner:     │ ┌─ Solve ──────────────────────────────────────────┐   │
│ [Dijkstra ▼] │ │  [Solve] [Run benchmark]  [Step-by-step]          │   │
│ [Solve]      │ └───────────────────────────────────────────────────┘   │
│ [Benchmark]  │                                                          │
│              │ ┌─ Metrics ─────────────────────────────────────────┐   │
│              │ │  [Bar charts] path length, time, nodes            │   │
│              │ └───────────────────────────────────────────────────┘   │
│              │ ┌─ Convergence (RRT*) ─────────────────────────────┐   │
│              │ │  [Line chart]                                     │   │
│              │ └───────────────────────────────────────────────────┘   │
└──────────────┴──────────────────────────────────────────────────────────┘
```

## 5.2 Integration with Existing Components

| Existing | Change |
|----------|--------|
| `index.html` | Add Generate panel, Solve/Benchmark buttons, planner dropdown; keep Examples + drop zone |
| `MapRenderer` | No change; receives `occupancy` from JS generator or loaded JSON |
| `js/parser.js` | No change; still parse benchmark JSON, visualization JSON, CSV |
| `js/charts.js` | No change; same `metricsBar`, `convergence` calls |
| `css/styles.css` | Add styles for new panels, buttons |
| CDN Chart.js | Bundle local copy for offline (e.g. `web/lib/chart.min.js`) |

## 5.3 State Management

- **Global state:** `{ environment, start, goal, path, planner, metrics }`
- **Flow:** Generate/Load → sets environment, start, goal → Solve/Benchmark → sets path, metrics
- **Persistence:** Optional localStorage for last generated map params (not required for MVP)

---

# 6. Phased Implementation

## Phase 1: MVP (Grid + 2–3 Algorithms)

**Scope:**
- Map generation: `random_uniform`, `maze` in pure JS
- Planners: Dijkstra, A* (required); optionally RRT or RRT*
- UI: Generate panel, Solve button, planner dropdown
- Reuse: MapRenderer, charts, parser for loaded files

**Deliverables:**
- `js/map-generator.js` — MapGenerator (random_uniform, maze)
- `js/planners/dijkstra.js`, `js/planners/astar.js`
- `js/grid-environment.js` — collision checking, bounds (thin wrapper over occupancy)
- `js/planner-runner.js` — single solve with timing
- UI updates in `index.html`, `css/styles.css`

**Estimated effort:** 3–5 days (1 dev)

## Phase 2: Full Planner Set

**Scope:**
- Add: Weighted A*, Theta*, PRM, Lazy PRM, RRT, RRT*, Informed RRT*
- Benchmark mode: Run all, aggregate, display comparison
- Optional: Step-by-step visualization for A*, RRT*

**Deliverables:**
- `js/planners/*.js` for each planner
- `js/benchmark-runner.js` — sequential or Web Worker parallel
- Benchmark UI: "Run benchmark" → table + charts
- Convergence chart for Informed RRT*

**Estimated effort:** 5–8 days

## Phase 3: Polish & Offline

**Scope:**
- Bundle Chart.js locally for offline
- Configurable start/goal (click on map)
- Export benchmark results as JSON/CSV download
- Optional: Emscripten path if perf needed

**Deliverables:**
- `web/lib/chart.min.js`
- Click-to-set start/goal on canvas
- Export button for results
- Emscripten build (optional)

**Estimated effort:** 2–3 days

---

# 7. Implementation Notes

## 7.1 Map Generator Ports

**Random Uniform:** Direct port from `map_generator.cpp::generate_random_uniform`. Use `seedrandom` or built-in `Math.random` with seeded PRNG (e.g. `mulberry32`). Keep (0,0) and (width-1, height-1) free.

**Maze (Kruskal):** Port Union-Find + edge shuffle. Grid size: `2*cellsHigh+1` × `2*cellsWide+1`. Room cells at odd indices; walls removed per MST edge.

## 7.2 Planner Ports

**Dijkstra / A*:** 8-connected grid. Min-heap (e.g. simple array + sort, or use a small heap lib). State = `{x, y}`. Cost = Euclidean or Manhattan depending on diagonal moves.

**RRT/RRT*:** Steer function for grid (snap to nearest free cell in direction). Collision: Bresenham line check against occupancy. RRT* rewiring: consider neighbors within radius; prune if better path.

## 7.3 Seeded RNG

For reproducible maps, use a seeded PRNG. Example (Mulberry32):

```javascript
function mulberry32(seed) {
  return function() {
    let t = seed += 0x6D2B79F5;
    t = Math.imul(t ^ t >>> 15, t | 1);
    t ^= t + Math.imul(t ^ t >>> 7, t | 61);
    return ((t ^ t >>> 14) >>> 0) / 4294967296;
  };
}
```

## 7.4 Performance Considerations

- For grids &lt; 100×100: Pure JS is sufficient.
- For 200×200+: Consider Web Workers to avoid UI freeze.
- RRT* 5000 iters: typically 50–200ms in JS for small grids.

---

# 8. Criteria of Completeness (Checklist)

- [x] Map generation in browser (random_uniform, maze) without backend
- [x] At least 2 planners (Dijkstra, A*) running in browser
- [x] Solve button: single planner, path + metrics displayed
- [ ] Benchmark mode: run multiple planners, comparison table/charts
- [x] No Python/C++ server required for core flows
- [ ] Works offline after initial load (Chart.js bundled)
- [x] Backward compatible: still load JSON/CSV from export/benchmark
- [x] Integration with existing `web/` (index.html, MapRenderer, charts)

---

# 9. Open Questions

1. **Web Workers:** Use for benchmark from Phase 1, or only in Phase 2 when many planners?
2. **Step-by-step:** Implement in Phase 2 or defer to Phase 3?
3. **Start/goal:** Configurable in MVP or Phase 3?
4. **Maze start/goal:** Default to `[1,1]` and `[grid_h-2, grid_w-2]` for maze type (grid coord space)?

---

# Appendix A: Review Checklist (Orchestrator)

| Requirement | Section | Status |
|-------------|---------|--------|
| Technical spec / implementation plan | Full document | ✅ |
| Technology options (A) Pure JS, (B) Emscripten | §2 | ✅ |
| User flows: map gen → single planner → benchmark | §3 | ✅ |
| Data structures, APIs | §4 | ✅ |
| UI layout | §5 | ✅ |
| Phased MVP → full | §6 | ✅ |
| Integration with existing web/ | §5.2, §6 | ✅ |

---

# 10. References

- Existing TZ: `docs/web_visualization_tz.md`
- C++ MapGenerator: `src/environment/map_generator.cpp`
- C++ planners: `include/planners/`, `src/planners/`
- Export script: `examples/export_visualization_data.py`
- Config format: `experiments/configs/simple_grid.json`, `maze.json`
