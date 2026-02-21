# Техническое задание

# Planning Benchmark Suite (C++ Research-Grade Motion Planning Framework)

---

# 1. Цель проекта

Разработать высокопроизводительный C++ фреймворк для:

* реализации и сравнения алгоритмов планирования движения:

  * Dijkstra
  * A*
  * Weighted A*
  * Theta*
  * PRM
  * RRT
  * RRT*
  * Informed RRT*
* генерации параметризуемых сред
* автоматизированного бенчмаркинга
* статистического анализа
* воспроизводимых экспериментов
* визуализации результатов

Проект должен соответствовать стандартам research-software и быть пригодным для магистерской диссертации и публикации.

---

# 2. Технологический стек

Язык:

* C++20

Сборка:

* CMake
* Conan/vcpkg

Библиотеки:

* Eigen — линейная алгебра
* Boost — графы и heap
* CGAL — геометрия и коллизии
* Google Test — unit tests
* nlohmann/json — конфигурации
* pybind11 — Python API для анализа

Опционально:

* OpenMP / TBB — параллелизация
* nanoflann — KD-tree

---

# 3. Архитектурные требования

## 3.1 Принципы

* Clean Architecture
* SOLID
* Разделение domain / infrastructure
* Compile-time polymorphism (templates)
* Zero-overhead abstraction
* Reproducibility-first design

---

## 4. Структура проекта

```
planning-benchmark/
│
├── include/
│   ├── core/
│   ├── environment/
│   ├── planners/
│   ├── geometry/
│   ├── metrics/
│   ├── benchmark/
│
├── src/
├── apps/
├── experiments/
├── tests/
├── docker/
└── docs/
```

---

# 5. Модель среды

## 5.1 Поддерживаемые пространства

1. 2D Grid (occupancy grid)
2. Continuous 2D
3. SE(2) (позиция + ориентация)
4. Опционально: SE(3)

---

## 5.2 Генератор карт

Параметры:

* size
* obstacle density
* narrow passage width
* clustering factor
* seed

Типы генерации:

* Random uniform
* Perlin noise
* Maze
* Narrow passage benchmark
* Random convex polygons

Требования:

* deterministic при заданном seed
* сериализация в JSON

---

# 6. Алгоритмы

Все алгоритмы должны реализовывать единый интерфейс:

```cpp
class IPlanner {
public:
    virtual Path solve(const Environment&,
                       const State& start,
                       const State& goal) = 0;
};
```

---

## 6.1 Grid-based

* Dijkstra
* A* (Manhattan, Euclidean, Diagonal)
* Weighted A*
* Theta*

Требования:

* min-heap
* проверка admissibility эвристики
* подсчет expansions

---

## 6.2 Sampling-based

* PRM
* Lazy PRM
* RRT
* RRT*
* Informed RRT*

Требования:

* KD-tree для nearest neighbor
* rewiring
* асимптотическая оптимальность
* configurable step size
* goal bias
* convergence tracking

---

# 7. Collision Checking

## Grid

* occupancy check
* Bresenham line

## Continuous

* проверка сегмент–полигон
* clearance computation
* минимальная дистанция до препятствий

---

# 8. Метрики

Обязательные:

1. Path length
2. Computation time (ms)
3. Nodes expanded
4. Success rate
5. Memory usage

---

## 8.1 Smoothness

[
S = \sum | \theta_i - \theta_{i-1} |
]

---

## 8.2 Clearance

Минимальная дистанция пути до препятствия.

---

## 8.3 Energy Functional

[
E = \int \kappa^2 ds
]

---

## 8.4 Convergence (для RRT*)

* cost vs iteration
* gap to optimal
* log convergence rate

---

# 9. Benchmark Engine

Требования:

* Batch запуск
* Параллельные эксперименты
* 30+ повторов на конфигурацию
* Автоматический подсчет:

  * mean
  * std
  * confidence intervals
* Экспорт:

  * CSV
  * JSON

CLI:

```
./benchmark --config config.json
```

---

# 10. Экспериментальный дизайн (Research Level)

## Контролируемые переменные

* map size
* obstacle density
* narrow passage width
* dimension
* iteration budget

---

## Статистика

* t-test
* ANOVA
* effect size
* boxplots
* convergence curves

---

# 11. Теоретический анализ

Включить:

* Асимптотическую сложность
* Теорию оптимальности RRT*
* Анализ эвристик A*
* Влияние размерности на sampling-based методы

---

# 12. Производительность

Минимальные требования:

* 100x100 grid A* < 50ms
* RRT* 5000 iter < 500ms
* 100 экспериментов < 1 мин (parallel)

---

# 13. Воспроизводимость

Обязательно:

* фиксированные seeds
* сохранение конфигураций
* Docker
* CI performance regression
* автоматическое сравнение коммитов

---

# 14. Тестирование

* Unit tests
* Property-based tests
* Regression tests
* Performance tests

Coverage ≥ 80%

---

# 15. Документация

* Архитектурная диаграмма
* Doxygen
* README с результатами
* Примеры конфигов
* Репликация экспериментов

---

# 16. Критерии завершенности

✔ Реализованы ≥ 8 алгоритмов
✔ Поддержка grid + continuous
✔ Автоматический benchmark
✔ Статистический анализ
✔ Репликабельность
✔ CI
✔ Документированная архитектура
✔ Экспериментальная глава уровня магистерской

---

# 17. Исследовательская гипотеза (пример)

> Sampling-based методы демонстрируют экспоненциальную деградацию в средах с узкими проходами, тогда как grid-based алгоритмы деградируют линейно по плотности препятствий.

Подтвердить через экспериментальный дизайн.

---

# 18. Потенциал публикации

При добавлении:

* Adaptive sampling strategy
* Learned heuristic
* Hybrid planner (grid + sampling)
* Dimension-aware radius adaptation

Проект может быть расширен до статьи уровня:

* IEEE ICRA
* IEEE IROS
* RA-L

(при полноценном теоретическом и экспериментальном обосновании)

---

# 19. Итог

Это уже:

* не просто pet-проект
* а полноценная research-платформа
* с архитектурой уровня robotics middleware
* с научной составляющей
* с возможностью публикации

---