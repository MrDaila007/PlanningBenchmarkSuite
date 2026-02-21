# Задача 6.1: pybind11 bindings для планировщиков и окружений

## Связь с юзер-кейсами
- UC-08: Визуализация результатов
- UC-09: Статистический анализ

## Цель задачи
Создать Python bindings через pybind11 для доступа к планировщикам, окружениям, результатам бенчмарков. Включить экспорт данных для matplotlib, scipy.

## Описание изменений

### Новые файлы
- `include/bindings/python_bindings.hpp`
- `src/bindings/python_bindings.cpp` — PYBIND11_MODULE
- Обновить CMakeLists.txt — pybind11, add_subdirectory или pybind11_add_module

### Expose в Python

**Модуль:** `planning_benchmark` или `pbs`

**Классы:**
- `State` — конструктор (x, y), (x, y, theta)
- `Path` — states, success, length
- `GridEnvironment` — load(file), width(), height(), from_json(str)
- `MapGenerator` — generate(params)
- `DijkstraPlanner`, `AStarPlanner`, `RRTPlanner`, `RRTStarPlanner`, и др.
- `planner.solve(env, start, goal)` → Path

**Benchmark:**
- `run_benchmark(config_path)` → DataFrame-like или dict с результатами
- Экспорт в pandas-compatible format (list of dicts)

**Утилиты:**
- `load_environment(path)` — загрузка из JSON
- `run_experiment(env, planner, start, goal, repeats)` → metrics dict

### Совместимость с pandas/matplotlib
- Результаты как list of dict или можно конвертировать в pd.DataFrame
- Path.states как list of (x,y) для построения графиков

## Тест-кейсы

### End-to-end тесты
1. **TC-E2E-01:** Python import и базовый solve
   ```python
   import planning_benchmark as pbs
   env = pbs.GridEnvironment(10, 10)  # или load
   planner = pbs.AStarPlanner()
   path = planner.solve(env, pbs.State(0,0), pbs.State(9,9))
   assert path.success
   ```

2. **TC-E2E-02:** Run benchmark из Python
   ```python
   results = pbs.run_benchmark("config.json")
   # results — dict или DataFrame
   ```

### Регрессионные тесты
- Все предыдущие тесты проходят
- C++ тесты не сломаны

## Критерии приёмки
- [ ] pybind11 модуль собирается
- [ ] State, Path, Environment, Planners доступны из Python
- [ ] run_benchmark работает
- [ ] Пример скрипта визуализации (boxplot, convergence curve) в examples/
- [ ] Все тесты проходят
