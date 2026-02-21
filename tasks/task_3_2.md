# Задача 3.2: Weighted A* и Theta*

## Связь с юзер-кейсами
- UC-01: Запуск планировщика на grid-карте

## Цель задачи
Реализовать Weighted A* (A* с весовой w>1 на эвристику) и Theta* (any-angle path planning).

## Описание изменений

### Новые файлы
- `include/planners/weighted_astar.hpp`, `src/planners/weighted_astar.cpp`
- `include/planners/thetastar.hpp`, `src/planners/thetastar.cpp`

### WeightedAStarPlanner

**Формула:** f = g + w * h, где w >= 1 (обычно 1.5–2.0)
- w=1 — обычный A*
- w>1 — suboptimal, но быстрее (меньше expansions)
- Параметр w в конструкторе

**Реализация:**
- Наследование от A* или общий базовый класс с параметром w
- nodes_expanded, path length

### ThetaStarPlanner

**Идея:** Any-angle planning — вершины пути не привязаны к узлам grid, линии напрямую к parent если line_of_sight.
- Parent не обязательно сосед по grid
- При расширении: проверять line_of_sight(current, parent_of_neighbor) и line_of_sight(neighbor, goal)
- Обновлять parent если путь через current короче
- Алгоритм Theta* (Lazy Theta* или полный Theta*)

**Ключевое:**
- Если line_of_sight(parent(s), s') свободна — cost = g(parent) + dist(parent, s')
- Иначе — как A*: g(s) + cost(s, s')

**Результат:**
- Более короткие и визуально приятные пути (меньше "ступенек")
- nodes_expanded

## Тест-кейсы

### Модульные тесты
1. **TC-UNIT-01:** Weighted A* — путь длиннее A* при w>1
   - Сравнить path length A* (w=1) и Weighted A* (w=2)
   - Weighted A* может дать suboptimal путь

2. **TC-UNIT-02:** Weighted A* — меньше expansions при w>1
   - На той же карте
   - nodes_expanded(Weighted) <= nodes_expanded(A*)

3. **TC-UNIT-03:** Theta* — путь короче A* при наличии диагоналей
   - Карта с открытым пространством
   - Theta* path length <= A* path length (на grid с 8-связностью)

4. **TC-UNIT-04:** Theta* — line_of_sight используется
   - Узкий проход
   - Путь корректно огибает препятствия

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] Weighted A* с настраиваемым w
- [ ] Theta* с any-angle путями
- [ ] nodes_expanded для обоих
- [ ] Все тесты проходят
