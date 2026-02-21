# Задача 5.3: CLI benchmark (./benchmark --config)

## Связь с юзер-кейсами
- UC-06: Пакетный запуск бенчмарков

## Цель задачи
Создать исполняемый файл benchmark с CLI: `./benchmark --config config.json [--output results.json] [--jobs N]`.

## Описание изменений

### Новые файлы
- `apps/benchmark/main.cpp`
- Обновить `CMakeLists.txt` — add_executable(benchmark ...)

### CLI аргументы

- `--config` (обязательный) — путь к config.json
- `--output` (опционально) — путь для результатов. Default: `results_<timestamp>.json` или вывод в stdout
- `--jobs` (опционально) — число параллельных потоков. Default: hardware_concurrency
- `--format` (опционально) — csv, json, both. Default: both
- `--save-config` (опционально) — сохранить использованный конфиг рядом с результатами

### Логика main.cpp

1. Парсинг аргументов (getopt или вручную)
2. Загрузка config.json
3. Создание BenchmarkEngine
4. Запуск engine.run(config)
5. Экспорт в указанный output (JSON, CSV)
6. Exit code: 0 при успехе, 1 при ошибке (невалидный конфиг, файл не найден)
7. Обработка Ctrl+C: сохранить частичные результаты (если возможно)

### Обработка ошибок
- Файл конфига не найден → сообщение, exit 1
- Невалидный JSON → сообщение с деталями, exit 1
- Ошибка при создании окружения/планировщика → сообщение, exit 1

## Тест-кейсы

### End-to-end тесты
1. **TC-E2E-01:** Запуск с валидным конфигом
   - `./benchmark --config tests/fixtures/sample_config.json`
   - Exit 0, создан output файл

2. **TC-E2E-02:** Несуществующий конфиг
   - `./benchmark --config nonexistent.json`
   - Exit 1, сообщение об ошибке

3. **TC-E2E-03:** --output указан
   - Результаты записаны в указанный файл

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] CLI: ./benchmark --config config.json
- [ ] --output, --jobs, --format, --save-config
- [ ] Корректный exit code
- [ ] Создан sample_config.json в tests/fixtures/
- [ ] Все тесты проходят
