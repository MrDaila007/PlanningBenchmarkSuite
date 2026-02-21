# Отчёт о тестировании: Задачи 1.1, 1.2, 2.1, 2.2, 3.1

**Дата:** 2025-02-21  
**Задачи:** 1.1 (Структура), 1.2 (Core), 2.1 (Grid collision), 2.2 (MapGenerator), 3.1 (Dijkstra, A*)

---

## Результаты тестирования

### Тесты: 19/19 прошли ✅

| Категория | Тесты | Статус |
|-----------|-------|--------|
| Structure | StateStoresXY, DijkstraFindsPathOnEmptyGrid, PathEmpty | ✅ |
| Core | PathComputeLength, PathEmptyAndSuccess, StateComparison | ✅ |
| GridCollision | IsOccupiedOnEmptyMap, IsOccupiedWithObstacle, LineOfSight*, GridEnvironment* | ✅ |
| MapGenerator | Determinism, JSONRoundTrip | ✅ |
| Planners | DijkstraEmptyMap, DijkstraWithObstacle, AStarFindsPath, AStarVsDijkstraExpansions | ✅ |

### Соответствие постановке

- **1.1:** CMake, заглушки модулей, зависимости — ✅
- **1.2:** State, Path, IPlanner, IEnvironment — ✅
- **2.1:** GridCollisionChecker, Bresenham — ✅
- **2.2:** MapGenerator (RandomUniform), JSON — ✅
- **3.1:** Dijkstra, A* (min-heap, 8-connected) — ✅

### Критерии приёмки

| Критерий | Статус |
|----------|--------|
| Dijkstra и A* реализованы | ✅ |
| Min-heap (priority_queue) | ✅ |
| nodes_expanded | ✅ |
| 8-связность | ✅ |
| Admissibility (A* Diagonal для 8-связности) | ✅ |

---

## Итог

✅ **КОД УТВЕРЖДЁН** для задач 1.1–3.1. Все тесты проходят, реализация соответствует ТЗ и архитектуре.
