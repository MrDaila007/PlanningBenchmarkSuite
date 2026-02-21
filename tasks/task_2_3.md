# Задача 2.3: Continuous collision (segment-polygon, clearance)

## Связь с юзер-кейсами
- UC-04: Collision Checking (Continuous)
- UC-02: Sampling-based планировщики

## Цель задачи
Реализовать проверку коллизий для непрерывного 2D: пересечение отрезка с полигонами, вычисление clearance.

## Описание изменений

### Новые файлы
- `include/geometry/continuous_collision_checker.hpp`
- `include/geometry/polygon.hpp` — примитив полигона (вершины, ccw)
- `src/geometry/continuous_collision_checker.cpp`
- `src/geometry/polygon.cpp`

### Класс Polygon
- `std::vector<Point2D> vertices` — Point2D {x, y}
- Вершины упорядочены (CCW для внешней границы)
- Методы: `contains(Point2D)`, `bounding_box()`

### Класс ContinuousCollisionChecker
- Конструктор: `vector<Polygon> obstacles`
- `bool segment_intersects_obstacles(Point2D a, Point2D b)` — пересекает ли отрезок ab любой препятствие
- `double clearance_at(Point2D p)` — минимальная дистанция от p до границы любого препятствия

### Алгоритмы
- Segment-polygon: проверка пересечения отрезка с каждым ребром полигона (segment-segment intersection)
- Point in polygon: ray casting или winding number
- Clearance: минимальная дистанция от точки до всех рёбер полигонов (point-to-segment distance)

### Интеграция
- Использовать Eigen для векторов/точек или простые struct {x,y}
- CGAL опционально: можно начать с собственной реализации для 2D

## Тест-кейсы

### Модульные тесты
1. **TC-UNIT-01:** segment_intersects — отрезок не пересекает квадрат
   - Квадрат [0,1]x[0,1], отрезок (2,0)-(2,1)
   - Ожидаемый результат: false

2. **TC-UNIT-02:** segment_intersects — отрезок пересекает квадрат
   - Квадрат [0,1]x[0,1], отрезок (0.5,-1)-(0.5,2)
   - Ожидаемый результат: true

3. **TC-UNIT-03:** clearance — точка вне препятствия
   - Точка (2,2), квадрат [0,1]x[0,1]
   - Ожидаемый результат: clearance ≈ 1.0 (до ближайшей границы)

4. **TC-UNIT-04:** point in polygon
   - Точка внутри треугольника
   - Ожидаемый результат: contains == true

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] Segment-polygon intersection работает
- [ ] Clearance вычисляется для точки вне препятствий
- [ ] Все тесты проходят
