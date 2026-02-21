# Задача 1.1: Создание структуры CMake, зависимостей и заглушек модулей

## Связь с юзер-кейсами
- UC-10: Тестирование и CI

## Цель задачи
Создать базовую структуру проекта, CMake-конфигурацию, подключить зависимости (Eigen, Boost, nlohmann/json, Google Test) и заглушки для всех модулей include/.

## Описание изменений

### Новые файлы
- `CMakeLists.txt` — корневой CMake с C++20, опции vcpkg/Conan
- `include/core/types.hpp` — заглушка: базовые типы (пока пусто или placeholder)
- `include/core/state.hpp` — заглушка: struct State (x, y)
- `include/core/path.hpp` — заглушка: struct Path (vector states, success)
- `include/core/iplanner.hpp` — заглушка: class IPlanner с solve() возвращающим пустой Path
- `include/environment/ienvironment.hpp` — заглушка: интерфейс IEnvironment
- `include/geometry/collision_checker.hpp` — заглушка: класс для collision
- `include/planners/dijkstra.hpp` — заглушка: класс DijkstraPlanner : IPlanner
- `include/planners/astar.hpp` — заглушка: AStarPlanner
- `include/metrics/metrics.hpp` — заглушка: struct Metrics
- `include/benchmark/benchmark_engine.hpp` — заглушка: класс BenchmarkEngine
- `src/core/state.cpp` (если нужны cpp) — реализация State (минимальная)
- `src/core/path.cpp` — реализация Path
- `tests/CMakeLists.txt` — подключение Google Test
- `tests/test_structure.cpp` — тест, что проект собирается и заглушки существуют

### Структура каталогов
```
planning-benchmark/
├── CMakeLists.txt
├── include/
│   ├── core/
│   ├── environment/
│   ├── planners/
│   ├── geometry/
│   ├── metrics/
│   └── benchmark/
├── src/
├── apps/
├── tests/
├── experiments/
├── docker/
└── docs/
```

### CMakeLists.txt
- project(PlanningBenchmarkSuite VERSION 0.1 LANGUAGES CXX)
- set(CMAKE_CXX_STANDARD 20)
- find_package для Eigen3, Boost, nlohmann_json, GTest
- add_library(planning_benchmark ...) с include из include/
- add_executable для будущего benchmark CLI (пока заглушка или пустой main)
- enable_testing(), add_subdirectory(tests)

### Заглушки
- IPlanner::solve() — return Path{} с success=false
- IEnvironment — pure virtual, пока без реализации
- Все include должны компилироваться без ошибок

## Тест-кейсы

### End-to-end тесты
1. **TC-E2E-01:** Сборка проекта
   - Команда: cmake .. && make
   - Ожидаемый результат: успешная сборка без ошибок

### Модульные тесты
1. **TC-UNIT-01:** Заглушка IPlanner возвращает пустой Path
   - Тестируемый код: любой Planner::solve()
   - Входные данные: любое окружение, start, goal (заглушки)
   - Ожидаемый результат: Path с success=false, states пусто

2. **TC-UNIT-02:** State хранит x, y
   - Входные данные: State s{1.0, 2.0}
   - Ожидаемый результат: s.x==1.0, s.y==2.0

### Регрессионные тесты
- Запуск всех тестов из tests/ (пока 2–3 теста)

## Критерии приёмки
- [ ] CMake конфигурирует проект с C++20
- [ ] Eigen, Boost, nlohmann_json, GTest найдены и подключены
- [ ] Все заглушки созданы и компилируются
- [ ] Тесты проходят
- [ ] Структура каталогов соответствует архитектуре

## Примечания
- Использовать vcpkg по умолчанию (или Conan — по решению пользователя)
- vcpkg.json или conanfile.txt создать для зависимостей
