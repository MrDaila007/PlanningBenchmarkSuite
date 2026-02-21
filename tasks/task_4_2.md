# Задача 4.2: RRT и RRT* (step size, goal bias, rewiring)

## Связь с юзер-кейсами
- UC-02: Sampling-based планировщики

## Цель задачи
Реализовать RRT и RRT* с configurable step size, goal bias, rewiring для RRT*.

## Описание изменений

### Новые файлы
- `include/planners/rrt.hpp`, `src/planners/rrt.cpp`
- `include/planners/rrt_star.hpp`, `src/planners/rrt_star.cpp`

### RRTPlanner

**Алгоритм:**
1. Инициализация: tree = {start}
2. Цикл (до max_iter или пока goal не достигнут):
   - С вероятностью goal_bias: sample = goal, иначе sample = random в bounds
   - Nearest: найти ближайшую вершину в tree (KD-tree)
   - Steer: от nearest к sample с ограничением step_size (если dist > step_size, обрезать)
   - Если collision_free(nearest, new_point): добавить new_point в tree, parent = nearest
   - Если new_point близко к goal (dist < threshold): считать успехом, построить path

**Параметры:**
- `step_size` (double)
- `goal_bias` (double, 0–1)
- `max_iter` (int)

**Результат:**
- Path от start до goal (или best effort)
- nodes_expanded = size of tree

### RRTStarPlanner

**Отличия от RRT:**
- Каждая вершина хранит cost-to-come (g-value)
- При добавлении new_point: искать parent с минимальным g(new_point) среди near neighbors (радиус r)
- **Rewiring:** После добавления new_point, для всех near neighbors: если g через new_point короче — обновить parent
- Радиус поиска r: зависит от dimension (формула RRT*), r = rrt_star_radius(n) = min(gamma * (log(n)/n)^(1/d), step_size * 2) или подобное
- Асимптотическая оптимальность при n→∞

**Параметры:**
- Те же что RRT + `rewiring_radius_factor` (gamma)

## Тест-кейсы

### Модульные тесты
1. **TC-UNIT-01:** RRT на пустом пространстве
   - Прямоугольник, start и goal
   - Путь найден за разумное число итераций

2. **TC-UNIT-02:** RRT с goal_bias
   - goal_bias=0.1 vs 0.3
   - Высокий goal_bias быстрее находит путь

3. **TC-UNIT-03:** RRT* — путь короче RRT при многих итерациях
   - 5000 итераций
   - RRT* path length <= RRT path length (в среднем)

4. **TC-UNIT-04:** RRT* rewiring
   - При добавлении новой точки, соседи переподключаются
   - Cost монотонно уменьшается при итерациях

### Performance
5. **TC-PERF-01:** RRT* 5000 iter < 500 ms

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] RRT и RRT* реализованы
- [ ] step_size, goal_bias configurable
- [ ] RRT* rewiring
- [ ] RRT* 5000 iter < 500 ms
- [ ] Все тесты проходят
