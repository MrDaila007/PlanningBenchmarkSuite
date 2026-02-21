# Задача 2.1: Collision checking для Grid (occupancy, Bresenham)

## Связь с юзер-кейсами
- UC-04: Collision Checking (Grid)

## Цель задачи
Реализовать проверку коллизий для occupancy grid: проверка ячейки, проверка линии по алгоритму Брезенхема.

## Описание изменений

### Новые файлы
- `include/geometry/grid_collision_checker.hpp` — класс GridCollisionChecker
- `src/geometry/grid_collision_checker.cpp` — реализация

### Класс GridCollisionChecker

**Конструктор:** принимает ссылку на occupancy grid (const std::vector<std::vector<int>> или flattened + width, height). 0 = свободно, 1 = занято.

**Методы:**
- `bool is_occupied(int row, int col) const` — проверка одной ячейки
- `bool is_cell_free(int row, int col) const` — !is_occupied (с проверкой границ)
- `bool line_of_sight(int r0, int c0, int r1, int c1) const` — проверка линии Брезенхема между (r0,c0) и (r1,c1). Возвращает true если все ячейки вдоль линии свободны.

**Bresenham:**
- Использовать алгоритм Брезенхема для растровых линий
- Проверять каждую ячейку на occupancy
- При первой занятой ячейке — вернуть false

### Интеграция
- GridCollisionChecker — отдельный утилитарный класс, не наследуется от IEnvironment
- Будущий GridEnvironment будет использовать GridCollisionChecker внутри

## Тест-кейсы

### Модульные тесты
1. **TC-UNIT-01:** is_occupied на пустой карте 10x10
   - Все ячейки свободны
   - is_occupied(0,0) == false, is_occupied(5,5) == false

2. **TC-UNIT-02:** is_occupied с препятствием
   - Установить (3,3)=1
   - is_occupied(3,3) == true

3. **TC-UNIT-03:** line_of_sight — прямая без препятствий
   - От (0,0) до (5,5)
   - Ожидаемый результат: true (если все ячейки свободны)

4. **TC-UNIT-04:** line_of_sight — препятствие на пути
   - Препятствие в (2,2), линия (0,0)-(4,4)
   - Ожидаемый результат: false

5. **TC-UNIT-05:** line_of_sight — горизонтальная и вертикальная линии
   - (0,0)-(9,0), (0,0)-(0,9)
   - Ожидаемый результат: true на пустой карте

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] GridCollisionChecker реализован
- [ ] Bresenham корректен для типичных линий
- [ ] Проверка границ (row, col в пределах [0, width) x [0, height))
- [ ] Все тесты проходят
