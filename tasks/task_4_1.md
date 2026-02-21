# Задача 4.1: PRM и Lazy PRM (KD-tree, базовый граф)

## Связь с юзер-кейсами
- UC-02: Sampling-based планировщики

## Цель задачи
Реализовать PRM (Probabilistic Roadmap) и Lazy PRM с KD-tree для nearest neighbor.

## Описание изменений

### Новые файлы
- `include/planners/prm.hpp`, `src/planners/prm.cpp`
- `include/planners/lazy_prm.hpp`, `src/planners/lazy_prm.cpp`
- Подключение nanoflann для KD-tree (или аналог)

### PRMPlanner

**Фазы:**
1. **Строительство:** Сэмплировать N точек (конфигурируемо) в свободном пространстве. Добавить start и goal.
2. **KD-tree:** Построить KD-tree по точкам (для 2D — 2D точки).
3. **Связи:** Для каждой точки — найти k ближайших соседей (или радиус r). Соединить ребром если segment collision-free.
4. **Запрос:** Dijkstra/A* на графе от start до goal.

**Параметры:**
- `num_samples` (int)
- `k_neighbors` (int) или `connection_radius` (double)
- `step_size` — для проверки segment (discretize или один вызов collision_check)

**Результат:**
- Path через граф
- nodes_expanded = число посещённых вершин в поиске на графе

### LazyPRMPlanner

**Отличие от PRM:**
- При построении графа — не проверять collision на этапе строительства
- Проверять collision "лениво" при запросе: при развёртке Dijkstra/A* проверять edge только когда впервые используем
- Кэшировать результат проверки
- Если edge в collision — удалить из графа (или не использовать) и продолжать поиск

**Преимущество:** Быстрее строительство, медленнее запрос (при большом числе запросов PRM выгоднее).

## Тест-кейсы

### Модульные тесты
1. **TC-UNIT-01:** PRM на простом окружении
   - Прямоугольник без препятствий
   - Start и goal на противоположных сторонах
   - Путь найден

2. **TC-UNIT-02:** PRM с препятствием
   - Препятствие между start и goal
   - Путь огибает препятствие

3. **TC-UNIT-03:** Lazy PRM — тот же результат что PRM
   - На одной конфигурации
   - Path length и success сопоставимы

4. **TC-UNIT-04:** KD-tree — nearest neighbors
   - Проверить что k nearest возвращаются корректно
   - Сравнить с brute-force на малой выборке

### Performance
5. **TC-PERF-01:** PRM 1000 samples, запрос < 100 ms

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] PRM и Lazy PRM реализованы
- [ ] KD-tree для nearest neighbor
- [ ] Configurable num_samples, k_neighbors
- [ ] Все тесты проходят
