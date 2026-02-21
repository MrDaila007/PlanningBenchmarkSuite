# Planning Benchmark Suite — статус разработки

**Проект:** C++ Research-Grade Motion Planning Framework  
**Дата начала:** 2025-02-21  
**Текущий этап:** Разработка

---

## Web-визуализация (отдельный workflow)

| Этап | Статус | Результат |
|------|--------|------------|
| 1. ТЗ (Аналитик) | ✅ | docs/web_visualization_tz.md |
| 2. Ревью ТЗ | ✅ | docs/web_visualization_tz_review.md |
| 3. Доработка ТЗ | ✅ | ТЗ обновлён по замечаниям |
| 4. Реализация MVP | ✅ | web/, export_visualization_data.py, Python bindings |
| 5. Ревью кода | ✅ | docs/web_visualization_impl_review.md |

---

## Autonomous Web (fully standalone, no backend)

| Этап | Статус | Результат |
|------|--------|-----------|
| 1. План / ТЗ (Аналитик) | ✅ | docs/autonomous_web_plan.md |
| 2. Ревью плана | ✅ | docs/autonomous_web_plan_review.md |
| 3. Реализация Phase 1 (MVP) | ✅ | map-generator, grid-environment, dijkstra, astar, UI |

---

## Этапы процесса

| Этап | Статус | Итерация | Результат |
|------|--------|----------|-----------|
| 1. Анализ | ✅ Выполнено | 1/2 | docs/technical_specification.md |
| 2. Ревью ТЗ | ✅ Выполнено | 1/2 | docs/tz_review.md |
| 3. Архитектура | ✅ Выполнено | 1/2 | docs/architecture.md |
| 4. Ревью архитектуры | ✅ Выполнено | 1/2 | docs/architecture_review.md |
| 5. Планирование | ✅ Выполнено | 1/1 | plan.md, tasks/*.md |
| 6. Ревью плана | ✅ Выполнено | 1/1 | plan_review.md |
| 7. Разработка | 🔄 В работе | Задачи 1.1–8.1 + Kruskal maze | 41/41 тестов |

---

## Задачи разработки

| Задача | Статус | Примечание |
|--------|--------|------------|
| 1.1 Структура проекта | ✅ | CMake, заглушки |
| 1.2 Core типы | ✅ | State, Path, IPlanner, IEnvironment |
| 2.1 Grid collision | ✅ | Bresenham, GridCollisionChecker |
| 2.2 MapGenerator, JSON | ✅ | RandomUniform, Maze (Kruskal), JSON round-trip |
| 2.3 Continuous collision | ✅ | Polygon, segment-polygon, clearance |
| 2.4 ContinuousEnv, SE2 | ✅ | ContinuousEnvironment, SE2Environment |
| 3.1 Dijkstra, A* | ✅ | Min-heap, 8-connected |
| 3.2 Weighted A*, Theta* | ✅ | w*A*, any-angle Theta* |
| 4.1 PRM, Lazy PRM | ✅ | KD-tree, k-NN, lazy collision |
| 4.2 RRT, RRT* | ✅ | step_size, goal_bias, rewiring |
| 4.3 Informed RRT* | ✅ | Ellipsoid sampling, convergence |
| 5.1 Metrics | ✅ | MetricsCollector |
| 5.2–5.3 Benchmark, CLI | ✅ | Batch run, JSON export |
| 6.1 pybind11 | ✅ | State, Path, planners, run_benchmark |
| 7.1–7.2 CI, Docker | ✅ | GitHub Actions, Dockerfile |
| 8.1 Документация | 🔄 | Doxygen, README, REPLICATION |

---

## Артефакты

| Артефакт | Файл | Статус |
|----------|------|--------|
| Исходная спецификация | TASK.md | ✅ Есть |
| Техническое задание | docs/technical_specification.md | ✅ Готово |
| Ревью ТЗ | docs/tz_review.md | ✅ Готово |
| Архитектура | docs/architecture.md | ✅ Готово |
| Ревью архитектуры | docs/architecture_review.md | ✅ Готово |
| План разработки | plan.md | ✅ Готово |
| Описания задач | tasks/task_*.md | ✅ 18 задач |
| Ревью плана | plan_review.md | ✅ Утверждён |
| CI | .github/workflows/ci.yml | ✅ |
| Docker | docker/Dockerfile | ✅ |
| ТЗ Web-визуализация | docs/web_visualization_tz.md | ✅ Готово |
| Ревью ТЗ Web-визуализация | docs/web_visualization_tz_review.md | ✅ Утверждено |
| Web UI (MVP) | web/index.html, js/, css/ | ✅ |
| Export script | examples/export_visualization_data.py | ✅ |
| Python to_json, MapGenerator | src/bindings/python_bindings.cpp | ✅ |
| Autonomous Web Plan | docs/autonomous_web_plan.md | ✅ |
| Autonomous Web Plan Review | docs/autonomous_web_plan_review.md | ✅ |
| MapGenerator (JS) | web/js/map-generator.js | ✅ |
| GridEnvironment (JS) | web/js/grid-environment.js | ✅ |
| Dijkstra/A* (JS) | web/js/planners/ | ✅ |
