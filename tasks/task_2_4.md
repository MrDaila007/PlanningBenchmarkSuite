# Задача 2.4: ContinuousEnvironment и SE2Environment

## Связь с юзер-кейсами
- UC-02: Sampling-based планировщики
- UC-11: SE(2) и опционально SE(3)

## Цель задачи
Реализовать ContinuousEnvironment (IEnvironment для непрерывного 2D) и SE2Environment (позиция + ориентация).

## Описание изменений

### Новые файлы
- `include/environment/continuous_environment.hpp`
- `include/environment/se2_environment.hpp`
- `src/environment/continuous_environment.cpp`
- `src/environment/se2_environment.cpp`

### Класс ContinuousEnvironment

**Наследование:** IEnvironment

**Атрибуты:**
- `obstacles_` (vector<Polygon>)
- `bounds_` (AABB: x_min, x_max, y_min, y_max)
- `ContinuousCollisionChecker checker_`

**Методы:**
- `is_valid(State s)` — в bounds, не внутри препятствия, не на границе (или на границе допустимо)
- `collision_free(State a, State b)` — segment_intersects через checker для (a.x,a.y)-(b.x,b.y)
- `clearance(State s)` — clearance_at(Point(a.x, a.y))

**Генерация:**
- Конструктор из vector<Polygon>
- Метод загрузки из JSON (координаты полигонов)
- MapGenerator для continuous: RandomConvexPolygons — генерация выпуклых полигонов

### Класс SE2Environment

**Наследование:** IEnvironment (или композиция с ContinuousEnvironment)

**Особенности SE(2):**
- State имеет x, y, theta
- Для collision: можно использовать footprint робота (прямоугольник/эллипс) при заданной ориентации
- Упрощение MVP: collision только по (x,y), theta влияет на метрики (smoothness), но не на collision
- Альтернатива: SE2Environment оборачивает ContinuousEnvironment и добавляет проверку ориентации

**Реализация MVP:**
- SE2Environment хранит ContinuousEnvironment
- is_valid: проверка (x,y) в базовом окружении; theta допустим [0, 2π)
- collision_free: делегирует в continuous (прямая линия в пространстве (x,y), theta пока не учитывается для collision)

## Тест-кейсы

### Модульные тесты
1. **TC-UNIT-01:** ContinuousEnvironment — точка в препятствии
   - Треугольник, точка внутри
   - is_valid == false

2. **TC-UNIT-02:** ContinuousEnvironment — collision_free
   - Два полигона с проходом между ними
   - Линия через проход → true

3. **TC-UNIT-03:** SE2Environment — базовая валидация
   - State с theta
   - is_valid проверяет (x,y) в bounds и не в препятствии

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] ContinuousEnvironment реализует IEnvironment
- [ ] SE2Environment поддерживает State с theta
- [ ] Загрузка continuous окружения из JSON
- [ ] Все тесты проходят
