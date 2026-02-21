# Задача 2.2: GridEnvironment и MapGenerator

## Связь с юзер-кейсами
- UC-03: Генерация параметризуемой карты
- UC-01: Запуск планировщика на grid-карте

## Цель задачи
Реализовать GridEnvironment (реализация IEnvironment для grid) и MapGenerator с детерминированной генерацией и JSON-сериализацией.

## Описание изменений

### Новые файлы
- `include/environment/grid_environment.hpp` — класс GridEnvironment
- `include/environment/map_generator.hpp` — класс MapGenerator
- `src/environment/grid_environment.cpp`
- `src/environment/map_generator.cpp`

### Класс GridEnvironment

**Наследование:** IEnvironment

**Атрибуты:**
- `width_, height_` (int)
- `occupancy_` (std::vector<std::vector<int>> или std::vector<uint8_t>)
- `GridCollisionChecker checker_` (или композиция)

**Методы IEnvironment:**
- `is_valid(State s)` — проверка: в пределах карты, ячейка свободна. Для State с (row,col) или (x,y) — преобразование к индексам.
- `collision_free(State a, State b)` — вызов line_of_sight через GridCollisionChecker
- `clearance(State s)` — для grid: минимальная манхэттенская/евклидова дистанция до занятой ячейки в окрестности (радиус 5–10). Или 0 если не реализовано в MVP.

**Дополнительно:**
- `int width() const`, `int height() const`
- `bool occupied(int row, int col) const`
- Загрузка из JSON, сохранение в JSON (nlohmann/json)

### Класс MapGenerator

**Параметры (структура MapGeneratorParams):**
- `width`, `height` (int)
- `obstacle_density` (double, 0–1)
- `narrow_passage_width` (int, опционально)
- `clustering_factor` (double, опционально)
- `seed` (uint64_t)

**Типы генерации (enum):** RandomUniform, PerlinNoise, Maze, NarrowPassage, RandomConvexPolygons

**Методы:**
- `GridEnvironment generate(params)` — детерминированно при одинаковом seed
- `void set_seed(uint64_t)` — установка seed

**Реализация RandomUniform (минимум для MVP):**
- Использовать std::mt19937 с seed
- Для каждой ячейки: случайное число < density → occupied
- Границы карты не заняты (или по договорённости)
- Углы (0,0) и (width-1, height-1) свободны для start/goal

**JSON:**
- Сериализация occupancy в JSON (массив массивов или flatten)
- Десериализация из JSON

## Тест-кейсы

### Модульные тесты
1. **TC-UNIT-01:** MapGenerator детерминированность
   - Сгенерировать карту с seed=42 дважды
   - Ожидаемый результат: идентичные occupancy

2. **TC-UNIT-02:** GridEnvironment::is_valid
   - Точка в препятствии → false
   - Точка за границами → false
   - Точка в свободной ячейке → true

3. **TC-UNIT-03:** GridEnvironment::collision_free
   - Использовать GridCollisionChecker через line_of_sight
   - Прямая без препятствий → true
   - Препятствие на пути → false

4. **TC-UNIT-04:** JSON round-trip
   - Сгенерировать карту, сериализовать в JSON, загрузить обратно
   - Ожидаемый результат: идентичная карта

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] GridEnvironment реализует IEnvironment
- [ ] MapGenerator генерирует детерминированно при seed
- [ ] RandomUniform реализован
- [ ] JSON сериализация/десериализация
- [ ] Все тесты проходят
