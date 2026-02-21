# Задача 7.1: Unit, regression, performance tests; coverage ≥ 80%

## Связь с юзер-кейсами
- UC-10: Тестирование и CI

## Цель задачи
Достичь полноценного покрытия тестами: unit, regression, performance. Coverage ≥ 80%. Performance baseline для регрессий.

## Описание изменений

### Структура тестов
- `tests/unit/` — модульные тесты по компонентам
- `tests/integration/` — интеграционные (planning pipeline)
- `tests/performance/` — performance benchmarks
- `tests/regression/` — регрессионные сценарии
- `tests/fixtures/` — конфиги, карты для тестов

### Unit tests
- Покрытие: core, geometry, environment, planners, metrics, benchmark
- Критичные пути: solve(), collision_check, metrics collection
- Граничные случаи: пустая карта, нет пути, невалидный start/goal

### Integration tests
- Полный пайплайн: load env → create planner → solve → collect metrics
- Разные комбинации planner + environment

### Performance tests
- 100x100 A* < 50 ms (baseline)
- RRT* 5000 iter < 500 ms (baseline)
- 100 экспериментов < 1 min (на 4 ядрах)
- Результаты логировать для CI (опционально fail при деградации >10%)

### Coverage
- gcov/lcov или similar
- Цель: ≥ 80% по строкам для src/
- Исключения: main.cpp, bindings (по согласованию)

### Регрессионные
- Набор стабильных сценариев с ожидаемыми результатами
- При изменении кода — все должны проходить
- Фиксы известных багов — добавить тест

## Тест-кейсы

### Обязательные
1. Все unit тесты из предыдущих задач
2. Integration: grid A* на 10x10, sampling RRT на continuous
3. Performance: зафиксировать baseline, не допускать деградацию
4. Coverage report: ≥ 80%

### Регрессионные тесты
- Все тесты из задач 1.1–6.1 продолжают проходить

## Критерии приёмки
- [ ] Unit tests для всех компонентов
- [ ] Integration tests
- [ ] Performance tests с baseline
- [ ] Coverage ≥ 80%
- [ ] Все тесты проходят
- [ ] Документирована команда запуска (ctest или make test)
