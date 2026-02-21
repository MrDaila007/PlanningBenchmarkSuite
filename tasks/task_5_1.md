# Задача 5.1: Metrics (path length, time, nodes, success rate, memory; smoothness, clearance, energy)

## Связь с юзер-кейсами
- UC-05: Сбор метрик планирования

## Цель задачи
Реализовать сбор всех метрик: обязательных и дополнительных (smoothness, clearance, energy, RRT* convergence).

## Описание изменений

### Новые файлы
- `include/metrics/metrics.hpp` — структура Metrics
- `include/metrics/metrics_collector.hpp` — класс для сбора метрик
- `src/metrics/metrics_collector.cpp`

### Структура Metrics

```cpp
struct Metrics {
    double path_length = 0.0;
    double computation_time_ms = 0.0;
    int nodes_expanded = 0;
    bool success = false;
    size_t memory_bytes = 0;  // опционально, через прирост heap

    // Дополнительные
    double smoothness = 0.0;   // S = sum(|theta_i - theta_{i-1}|)
    double clearance = 0.0;   // min distance to obstacles
    double energy = 0.0;      // E = integral(kappa^2 ds), curvature

    // RRT* specific
    std::vector<std::pair<int, double>> cost_vs_iteration;
    double gap_to_optimal = 0.0;
};
```

### MetricsCollector

**Методы:**
- `Metrics collect(Path path, double time_ms, int nodes, IEnvironment* env)` — собрать все метрики
- `path_length`: сумма dist(states[i], states[i+1]) или path.length
- `smoothness`: для Path с theta — sum |theta_i - theta_{i-1}|; для 2D точек — угол между соседними сегментами
- `clearance`: min по всем states env->clearance(s)
- `energy`: интеграл кривизны в квадрате. Кривизна в вершине i: угол между (states[i-1], states[i]) и (states[i], states[i+1]) / длина дуги. Приближение через дискретные точки.
- `memory`: опционально, через разницу heap до/после (platform-specific)

### Интеграция с планировщиками
- Каждый планировщик возвращает Path + может предоставлять nodes_expanded, time
- MetricsCollector вызывается после solve()
- RRT*/Informed RRT* передают convergence data

## Тест-кейсы

### Модульные тесты
1. **TC-UNIT-01:** path_length для прямой линии
   - 3 точки (0,0), (1,0), (2,0)
   - path_length == 2.0

2. **TC-UNIT-02:** smoothness для ломаной
   - 3 точки с углом 90°
   - smoothness == pi/2 (или в градусах)

3. **TC-UNIT-03:** clearance
   - Path вдали от препятствий
   - clearance > 0

4. **TC-UNIT-04:** success rate
   - 10 запусков, 8 успешных
   - success_rate == 0.8

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] Все обязательные метрики: path_length, time, nodes, success, memory
- [ ] smoothness, clearance, energy
- [ ] RRT* convergence (cost vs iter, gap)
- [ ] Все тесты проходят
