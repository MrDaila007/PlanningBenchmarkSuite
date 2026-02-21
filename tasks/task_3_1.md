# Задача 3.1: Dijkstra и A* (min-heap, admissibility check)

## Связь с юзер-кейсами
- UC-01: Запуск планировщика на grid-карте

## Цель задачи
Реализовать Dijkstra и A* для grid-окружения с min-heap, подсчётом expansions, проверкой admissibility эвристики.

## Описание изменений

### Новые файлы
- `include/planners/dijkstra.hpp`, `src/planners/dijkstra.cpp`
- `include/planners/astar.hpp`, `src/planners/astar.cpp`
- `include/planners/heuristic.hpp` — эвристики для A*

### DijkstraPlanner

**Алгоритм:**
- Min-heap (std::priority_queue или Boost.Heap) по cost (g-value)
- Открытый список: (cost, (row, col))
- Закрытый список: set (row, col)
- Соседи: 4-связность или 8-связность (опция)
- collision_free для перехода
- Подсчёт nodes_expanded (сколько раз извлекли из heap до goal)

**Результат:**
- Path с states от start до goal
- Метрики: nodes_expanded (доступно через getter или в результате)

### AStarPlanner

**Эвристики:** Manhattan, Euclidean, Diagonal (chebyshev)
- Manhattan: |dx|+|dy|
- Euclidean: sqrt(dx^2+dy^2)
- Diagonal: max(|dx|,|dy|) + (sqrt2-1)*min(|dx|,|dy|)

**Admissibility check:**
- Эвристика допустима, если h(n) <= cost(n, goal) для любой n
- Для grid: Manhattan допустима при 4-связности; Euclidean допустима при 8-связности
- В конструкторе: при задании эвристики проверить admissibility для выбранной связности
- При недопустимой — выдать warning или throw

**Алгоритм:**
- f = g + h, min-heap по f
- Остальное аналогично Dijkstra
- nodes_expanded — подсчёт

### Интеграция
- Оба реализуют IPlanner
- Принимают только GridEnvironment (проверка типа или общий IEnvironment с поддержкой grid)
- State с grid_pos (row, col) для grid

## Тест-кейсы

### Модульные тесты
1. **TC-UNIT-01:** Dijkstra на пустой карте 10x10
   - Start (0,0), goal (9,9)
   - Ожидаемый результат: путь найден, длина ~18 (4-связность) или ~9 (8-связность)

2. **TC-UNIT-02:** Dijkstra с препятствием
   - Стена по середине, обход
   - Путь длиннее прямого

3. **TC-UNIT-03:** A* — путь короче или равен Dijkstra
   - Та же карта
   - A* находит оптимальный путь

4. **TC-UNIT-04:** nodes_expanded
   - Dijkstra и A* на одной карте
   - A* обычно меньше expansions

5. **TC-UNIT-05:** Admissibility — Euclidean для 4-связности
   - Euclidean может переоценивать на 4-связности
   - Должен быть warning или отказ

### Performance
6. **TC-PERF-01:** 100x100 grid A* < 50 ms
   - Запуск 10 раз, среднее < 50 ms

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] Dijkstra и A* реализованы
- [ ] Min-heap используется
- [ ] nodes_expanded подсчитывается
- [ ] Admissibility check для A*
- [ ] 100x100 A* < 50 ms
- [ ] Все тесты проходят
