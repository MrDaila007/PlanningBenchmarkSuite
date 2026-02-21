# Задача 5.2: Benchmark Engine (batch, parallel, mean/std/CI, CSV/JSON export)

## Связь с юзер-кейсами
- UC-06: Пакетный запуск бенчмарков
- UC-07: Репродукция экспериментов

## Цель задачи
Реализовать Benchmark Engine: batch-запуск с 30+ повторами, параллельное выполнение, mean/std/CI, экспорт CSV и JSON. Валидация конфига, сохранение конфигов для воспроизводимости.

## Описание изменений

### Новые файлы
- `include/benchmark/benchmark_engine.hpp`, `src/benchmark/benchmark_engine.cpp`
- `include/benchmark/experiment_config.hpp` — парсинг конфига
- `include/benchmark/statistics.hpp` — mean, std, confidence interval

### BenchmarkEngine

**Вход:** ExperimentConfig (или путь к config.json)

**Конфиг (JSON):**
- experiments: массив { environment, planner, planner_params, start, goal, repeats, seed }
- environment: { type, width, height, generator, params, seed } или path к файлу
- Параметр version для обратной совместимости

**Логика:**
1. Парсинг и валидация config.json (обязательные поля, типы)
2. Для каждого эксперимента: repeats раз запустить planner.solve()
3. Параллелизация: OpenMP/TBB — параллелить по экспериментам или по повторениям (осторожно с seed)
4. Сбор Metrics для каждого run
5. Вычисление: mean, std для path_length, time_ms, nodes_expanded; success_rate
6. 95% CI: t-distribution при n>=30
7. Экспорт в JSON и CSV

### Statistics

- `mean(vector)`, `std(vector)`
- `confidence_interval_95(vector)` → (lower, upper)
- При n<30 — использовать t-distribution

### Воспроизводимость
- Сохранять config_hash (hash от сериализованного конфига) в результатах
- Сохранять полный конфиг рядом с результатами (опция --save-config)
- Seed передаётся в MapGenerator/Environment

## Тест-кейсы

### End-to-end тесты
1. **TC-E2E-01:** Полный benchmark run
   - Конфиг с 1 экспериментом, 30 повторов
   - Ожидаемый результат: JSON и CSV с mean, std, CI
   - success_rate в [0,1]

### Модульные тесты
2. **TC-UNIT-01:** mean, std, CI
   - Вектор [1,2,3,4,5]
   - mean=3, std≈1.58, CI содержит 3

3. **TC-UNIT-02:** Валидация конфига
   - Невалидный JSON → ошибка
   - Отсутствует обязательное поле → ошибка

4. **TC-UNIT-03:** Детерминированность при seed
   - Два запуска с одинаковым config (один эксперимент, 5 повторов, seed=42)
   - Ожидаемый результат: идентичные raw результаты (path_length и т.д.)

### Performance
5. **TC-PERF-01:** 100 экспериментов (100 пар env+planner, по 30 повторов) < 1 мин на 4+ ядрах
   - Упрощённый конфиг: мелкие карты, быстрые планировщики

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] Batch run с 30+ повторами
- [ ] mean, std, 95% CI
- [ ] Экспорт CSV, JSON
- [ ] Параллельное выполнение (OpenMP)
- [ ] Валидация config
- [ ] Сохранение конфига для воспроизводимости
- [ ] 100 экспериментов < 1 мин
- [ ] Все тесты проходят
