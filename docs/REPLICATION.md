# Инструкция по воспроизведению экспериментов

## 1. Установка зависимостей

### Ubuntu/Debian
```bash
sudo apt install build-essential cmake \
  libeigen3-dev nlohmann-json3-dev libgtest-dev
```

### Python bindings (опционально)
```bash
sudo apt install pybind11-dev python3-dev
```

## 2. Сборка

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## 3. Запуск бенчмарка

```bash
./benchmark --config ../experiments/configs/simple_grid.json
```

Результаты сохраняются в `simple_grid_results.json` и `simple_grid_results.csv`.

## 4. Ожидаемые результаты (seed=42)

Для `simple_grid.json` (20×20 сетка, density 0.1, A*, 5 repeats):
- success_rate: 1.0 (путь найден во всех повторах)
- mean_path_length: ~27–28
- mean_time_ms: < 10 ms

## 5. Docker

```bash
docker build -f docker/Dockerfile -t pbs .
docker run pbs ctest --output-on-failure
docker run pbs --config /workspace/experiments/configs/simple_grid.json
```

## 6. Параметры и воспроизводимость

- `seed` в конфиге обеспечивает детерминированную генерацию карты
- При одинаковом seed результаты воспроизводимы
- Для sampling-based (PRM, RRT*) используйте фиксированный seed в MapGenerator
