# Задача 4.3: Informed RRT* и convergence tracking

## Связь с юзер-кейсами
- UC-02: Sampling-based планировщики
- UC-05: Метрики (RRT* convergence)

## Цель задачи
Реализовать Informed RRT* (сэмплирование в эллипсоиде между start и goal) и сбор метрик сходимости.

## Описание изменений

### Новые файлы
- `include/planners/informed_rrt_star.hpp`, `src/planners/informed_rrt_star.cpp`

### InformedRRTStarPlanner

**Идея Informed RRT*:**
- После того как найден первый путь с cost c_best, сэмплировать только в эллипсоиде: {x: dist(start,x) + dist(x,goal) <= c_best}
- Это множество всех точек, через которые может проходить путь лучше текущего best
- Сэмплирование: генерировать точку в эллипсе (трансформация сферы в эллипс)
- Ускоряет сходимость к оптимуму

**Реализация:**
- Наследование от RRT*
- Переопределить sample(): если c_best < inf, сэмплировать в эллипсоиде; иначе — uniform в bounds
- Эллипс: фокусы в start и goal, сумма расстояний = c_best

### Convergence Tracking

**Метрики для RRT*:**
- `cost_vs_iteration`: вектор пар (iteration, best_cost)
- `gap_to_optimal`: best_cost - optimal_cost (optimal из A*/Dijkstra на grid или известный для тестовой сцены)
- `log_convergence_rate`: оценка скорости сходимости (линейная регрессия log(gap) vs log(iter))

**Сбор:**
- После каждой итерации: обновлять best_cost если найден лучший путь
- Сохранять историю для экспорта
- Опционально: callback или геттер для получения convergence data

**Структура:**
```cpp
struct ConvergenceData {
    std::vector<std::pair<int, double>> cost_vs_iteration;
    double final_cost;
    double gap_to_optimal;  // если optimal известен
};
```

## Тест-кейсы

### Модульные тесты
1. **TC-UNIT-01:** Informed RRT* — сходимость быстрее RRT*
   - Та же сцена
   - Informed RRT* достигает того же cost за меньше итераций (в среднем)

2. **TC-UNIT-02:** Convergence tracking — cost не возрастает
   - cost_vs_iteration монотонно неубывающий (best cost может только улучшаться)

3. **TC-UNIT-03:** Эллипсоидное сэмплирование
   - После первого пути, сэмплы лежат в эллипсоиде
   - Проверка геометрии

### Регрессионные тесты
- Все предыдущие тесты проходят

## Критерии приёмки
- [ ] Informed RRT* реализован
- [ ] Сэмплирование в эллипсоиде после первого пути
- [ ] Convergence tracking: cost vs iteration, gap to optimal
- [ ] Все тесты проходят
