# Техническое задание: Web-визуализация результатов Planning Benchmark Suite

**Версия:** 1.0  
**Дата:** 2025-02-21  
**Источник:** ТЗ основного проекта (docs/technical_specification.md), UC-08, UC-09  

---

# 1. Общее описание

## 1.1 Цель разработки

Создать веб-компонент визуализации для Planning Benchmark Suite, позволяющий интерактивно отображать окружения, пути, метрики и кривые сходимости планировщиков. Компонент должен интегрироваться с существующими выходными данными бенчмарка (JSON, CSV) и расширять возможности анализа для исследователей.

## 1.2 Связь с проектом

- **Planning Benchmark Suite** — C++ фреймворк для motion planning с планировщиками (Dijkstra, A*, RRT*, Informed RRT* и др.), окружениями (grid, continuous, SE2), benchmark engine, pybind11 Python API.
- **Существующие артефакты:** `*_results.json`, `*_results.csv`, конфиги `experiments/configs/*.json`, Python API с `convergence_data()`, `visualize_convergence.py`.
- **Целевые UC:** UC-08 (визуализация), UC-09 (статистический анализ).

## 1.3 Целевая аудитория

- Исследователь (магистр/PhD) — анализ экспериментов, сравнение планировщиков
- Разработчик алгоритмов — отладка и демонстрация работы планировщиков

---

# 2. Пользовательские сценарии (User Cases)

## UC-WV-01: Просмотр grid-карты и препятствий

**Актёры:** Исследователь, система.

**Предусловия:** Есть данные окружения (occupancy grid) в JSON или параметры для воссоздания.

**Основной сценарий:**
1. Исследователь загружает файл результатов или конфиг эксперимента
2. Система извлекает параметры окружения (width, height, obstacle_density, seed)
3. Система отображает 2D grid-карту: свободные клетки (например, белые), препятствия (чёрные/серые)
4. Исследователь видит start и goal на карте (если указаны в данных)

**Критерии приёмки:**
- Визуализация occupancy grid с масштабированием
- Поддержка формата `{width, height, occupancy: [[0|1,...],...]}` (GridEnvironment.to_json)
- Воссоздание карты из конфига (environment.type, width, height, generator, obstacle_density, seed) — только при наличии Python backend (Phase 2) или предэкспортированного JSON; для MVP — загрузка готового JSON с occupancy

---

## UC-WV-02: Визуализация пути на карте

**Актёры:** Исследователь, система.

**Предусловия:** Есть карта и данные пути (последовательность состояний).

**Основной сценарий:**
1. Исследователь загружает результат планирования с путём
2. Система отображает карту с наложенным путём (линия или последовательность точек)
3. Start и goal выделены (например, зелёный и красный маркеры)
4. Возможность переключения между несколькими путями (если есть несколько запусков)

**Критерии приёмки:**
- Отображение Path как polyline на grid или continuous пространстве
- Поддержка формата `{states: [{x,y},...], success, length}`
- Наглядное различие start/goal и промежуточных точек

---

## UC-WV-03: Отображение кривых сходимости (RRT*)

**Актёры:** Исследователь, система.

**Предусловия:** Есть данные `cost_vs_iteration` (Informed RRT* / RRT*).

**Основной сценарий:**
1. Исследователь загружает результат с convergence data
2. Система строит график: ось X — итерация, ось Y — best cost
3. Поддержка сравнения нескольких кривых (разные планировщики/параметры)
4. Экспорт графика в PNG/SVG

**Критерии приёмки:**
- Формат данных: `[[iteration, cost], ...]`
- Интерактивный график (зум, pan, tooltip)
- Сравнение до 5+ кривых на одном графике

---

## UC-WV-04: Сравнение метрик между планировщиками

**Актёры:** Исследователь, система.

**Предусловия:** Есть `*_results.json` или `*_results.csv` от benchmark.

**Основной сценарий:**
1. Исследователь загружает файл результатов (JSON/CSV)
2. Система парсит: planner, mean_path_length, std_path_length, mean_time_ms, mean_nodes, success_rate, ci_*
3. Система отображает boxplots или bar charts для выбранных метрик
4. Исследователь может выбрать метрики и способ визуализации

**Критерии приёмки:**
- Импорт `*_results.json` (формат BenchmarkEngine)
- Импорт `*_results.csv` (planner, mean_*, std_*, success_rate, ci_low, ci_high)
- Boxplot/bar для path_length, time_ms, nodes, success_rate
- Отображение confidence intervals

---

## UC-WV-05: Интеграция с существующим pipeline

**Актёры:** Исследователь, система.

**Основной сценарий:**
1. Исследователь запускает `./benchmark --config simple_grid.json`
2. Получает `simple_grid_results.json`, `simple_grid_results.csv`
3. Открывает веб-визуализатор, указывает путь к результатам
4. Система загружает данные и отображает метрики

**Критерии приёмки:**
- Работа с файлами, сгенерированными текущим BenchmarkEngine
- Опционально: загрузка конфига для воссоздания окружения (карта по seed)
- Совместимость с путями в файловой системе (local) и drag-and-drop

---

## UC-WV-06: Одиночный эксперимент с путём и картой

**Актёры:** Исследователь, система.

**Предусловие:** Данные из Python API (путь + карта) или расширенный JSON.

**Основной сценарий:**
1. Исследователь запускает скрипт: решает задачу через Python API, сохраняет JSON с path + env
2. Загружает JSON в веб-визуализатор
3. Видит карту + путь на карте + метрики

**Критерии приёмки:**
- Поддержка формата:
  ```json
  {
    "environment": { "width": 20, "height": 20, "occupancy": [...] },
    "path": { "states": [...], "success": true, "length": 25.5 },
    "start": [0, 0], "goal": [19, 19],
    "planner": "astar", "time_ms": 12.3, "nodes_expanded": 45
  }
  ```
- Экспорт такого JSON через Python-скрипт (документировать в ТЗ)

---

# 3. Требования

## 3.1 Функциональные

| ID | Требование | Приоритет |
|----|------------|-----------|
| F-01 | Визуализация occupancy grid (grid maps) | Критичный |
| F-02 | Визуализация препятствий и free space | Критичный |
| F-03 | Отображение пути (Path) на карте | Критичный |
| F-04 | Отображение start/goal | Критичный |
| F-05 | Графики метрик (path length, time, nodes, success rate) | Высокий |
| F-06 | Кривые сходимости (cost vs iteration) | Высокий |
| F-07 | Импорт JSON результатов benchmark | Критичный |
| F-08 | Импорт CSV результатов benchmark | Высокий |
| F-09 | Импорт расширенного JSON (env + path + metrics) | Высокий |
| F-10 | Boxplots, bar charts для сравнения планировщиков | Средний |
| F-11 | Экспорт графиков в PNG/SVG | Средний |
| F-12 | Интерактивность (зум, pan, tooltip) | Средний |

## 3.2 Нефункциональные

| ID | Требование |
|----|------------|
| NF-01 | Работа в современных браузерах (Chrome, Firefox, Safari, Edge) |
| NF-02 | Загрузка карт до 500×500 без заметной задержки |
| NF-03 | Без серверной логики для MVP (можно static files + client-side) |
| NF-04 | Совместимость с offline-режимом (все данные локально) |

---

# 4. Архитектура

## 4.1 Обзор

```
┌─────────────────────────────────────────────────────────────────┐
│                     Web Visualization Frontend                   │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │
│  │ Map View    │  │ Path View   │  │ Metrics / Convergence   │  │
│  │ (grid/env)  │  │ (on map)    │  │ Charts                  │  │
│  └─────────────┘  └─────────────┘  └─────────────────────────┘  │
├─────────────────────────────────────────────────────────────────┤
│  Data Loader: JSON (results, env+path), CSV (results)          │
└─────────────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────────────┐
│  Data Sources (Planning Benchmark Suite)                         │
├─────────────────────────────────────────────────────────────────┤
│  *_results.json     *_results.csv     config.json                │
│  (extended JSON with env+path from Python export script)          │
└─────────────────────────────────────────────────────────────────┘
```

## 4.2 Варианты технологии

### Вариант A: Static HTML + Vanilla JS + Charting Library (рекомендуется для MVP)

- **Стек:** HTML5, CSS3, JavaScript (ES6+), Canvas или SVG для карты; Chart.js или Plotly.js для графиков (локальные копии в `web/lib/` для offline-режима)
- **Плюсы:** Минимум зависимостей, легко встраивается в docs/, работает offline, не требует сборки
- **Минусы:** Модульность ограничена

### Вариант B: React + TypeScript + Recharts/Visx

- **Стек:** React 18, TypeScript, Vite, Recharts или Visx
- **Плюсы:** Компонентная архитектура, типизация, удобная расширяемость
- **Минусы:** Требует node/npm, сборка

### Вариант C: Python Backend (FastAPI/Flask) + фронт

- **Стек:** FastAPI + статический фронт (HTML/JS или React)
- **Плюсы:** Можно вызывать planning_benchmark (pybind11) для on-the-fly генерации карт/путей
- **Минусы:** Сложнее развёртывание, нужен Python runtime

**Рекомендация:** Вариант A для MVP; при необходимости масштабирования — переход на B. Вариант C — для Phase 2, если потребуется генерация данных на лету без предэкспорта.

## 4.3 Поток данных

```
1. Benchmark run:
   ./benchmark --config X.json  →  X_results.json, X_results.csv

2. (Опционально) Python export для path + env:
   python examples/export_visualization_data.py --config X.json --out viz_data.json
   → viz_data.json содержит: environment, path, start, goal, planner, metrics

3. Web UI:
   - User selects X_results.json OR viz_data.json OR drag-and-drop
   - Parser: detect format (benchmark results vs extended)
   - Render: map (if env data), path (if path data), charts (from results)
```

### Формат данных (расширенный JSON для визуализации)

Для отображения карты и пути в одном файле — расширенный формат (генерируется Python-скриптом):

```json
{
  "type": "visualization",
  "environment": {
    "type": "grid",
    "width": 20,
    "height": 20,
    "occupancy": [[0,0,1,...], ...]
  },
  "start": [0, 0],
  "goal": [19, 19],
  "path": {
    "states": [{"x": 0, "y": 0}, {"x": 1, "y": 0}, ...],
    "success": true,
    "length": 25.5
  },
  "planner": "astar",
  "time_ms": 12.3,
  "nodes_expanded": 45,
  "convergence": [[1, 50.0], [2, 48.2], ...]
}
```

Текущий `*_results.json` (без env/path) используется только для графиков метрик.

---

# 5. Детализация компонентов

## 5.1 Map Renderer

- **Вход:** `{width, height, occupancy}` (2D array, 0=free, 1=obstacle)
- **Выход:** Canvas/SVG с раскрашенными клетками
- **Масштаб:** Автомасштабирование под контейнер, сохранение aspect ratio

## 5.2 Path Overlay

- **Вход:** `path.states`, `start`, `goal`
- **Координаты:** Grid — (col, row) → (x, y). Continuous — (x, y) напрямую
- **Стиль:** Линия пути (например, синяя), start (зелёный), goal (красный)

## 5.3 Metrics Charts

- **Вход:** Массив `{planner, mean_path_length, std_path_length, mean_time_ms, ...}`
- **Типы:** Bar chart (по планировщикам), boxplot (если есть raw samples)
- **Библиотека:** Chart.js, Plotly.js или аналог

## 5.4 Convergence Chart

- **Вход:** `[[iteration, cost], ...]`
- **Тип:** Line chart
- **Опции:** Множественные линии для сравнения

---

# 6. Интеграция с Planning Benchmark Suite

## 6.1 Текущие форматы

**Benchmark JSON** (src/benchmark/benchmark_engine.cpp):

```json
{
  "results": [
    {
      "planner": "astar",
      "mean_path_length": 150.5,
      "std_path_length": 12.3,
      "mean_time_ms": 25.0,
      "std_time_ms": 1.2,
      "mean_nodes": 120.5,
      "success_rate": 0.95,
      "ci_path_length": [140, 161],
      "ci_time_ms": [23.1, 26.9],
      "repeats": 30
    }
  ]
}
```

**Benchmark CSV:** `planner,mean_path_length,std_path_length,mean_time_ms,...`

**GridEnvironment.to_json():**

```json
{"width": 20, "height": 20, "occupancy": [[0,0,1,...],[...],...]}
```

**Соглашение по координатам:** В конфиге и JSON `start`, `goal` заданы как `[x, y]`, где для grid `x` = col (столбец), `y` = row (строка), индексы с 0. Соответствует `State(col, row)` в C++.

## 6.2 Необходимые доработки (Phase 1)

1. **Python export script** — скрипт `examples/export_visualization_data.py`:
   - Загрузка конфига
   - Генерация окружения (MapGenerator) по параметрам
   - Один solve
   - Экспорт env.to_json(), path, convergence (если Informed RRT*) — метод `to_json()` уже реализован в C++ для GridEnvironment, требуется только экспорт в Python bindings
   - Сохранение в расширенный JSON

2. **Python bindings** — экспортировать `to_json()` для GridEnvironment в pybind11 (метод уже реализован в C++).

## 6.3 Опциональные доработки (Phase 2)

- Расширить BenchmarkEngine: опция `--save-sample-path` — сохранять один sample path + env в JSON для визуализации
- Экспорт convergence в JSON при использовании Informed RRT*

---

# 7. Структура артефактов

```
PlanningBenchmarkSuite/
├── docs/
│   ├── web_visualization_tz.md     # Этот документ
│   └── ...
├── web/
│   ├── index.html                  # Точка входа
│   ├── css/
│   │   └── styles.css
│   ├── js/
│   │   ├── parser.js               # Парсинг JSON/CSV
│   │   ├── map-renderer.js         # Отрисовка карты
│   │   ├── path-overlay.js        # Отрисовка пути
│   │   └── charts.js               # Графики метрик/сходимости
│   └── lib/                        # Chart.js, etc. (или CDN)
├── examples/
│   └── export_visualization_data.py # Скрипт экспорта env+path
└── ...
```

---

# 8. Критерии приёмки

- [ ] Визуализация grid-карты из JSON
- [ ] Отображение пути на карте (при наличии данных)
- [ ] Импорт `*_results.json` и построение графиков метрик
- [ ] Импорт `*_results.csv`
- [ ] Кривые сходимости для cost_vs_iteration
- [ ] Работа в браузере без сервера (static files)
- [ ] Python-скрипт экспорта расширенного JSON (env+path)
- [ ] Документация по использованию в README или docs/

---

# 9. Открытые вопросы

1. **Continuous / SE2:** Grid — приоритет MVP; визуализация continuous obstacles (polygons) — Phase 2?
2. **Сравнение путей:** Поддержка наложения нескольких путей (разные планировщики) на одну карту?
3. **RRT tree:** Визуализация дерева RRT/RRT* (опционально, Phase 2)?
