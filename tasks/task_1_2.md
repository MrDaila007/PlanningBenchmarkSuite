# Задача 1.2: Core типы и интерфейсы (State, Path, IPlanner, IEnvironment)

## Связь с юзер-кейсами
- UC-01: Запуск планировщика на grid-карте
- UC-02: Запуск sampling-based планировщика

## Цель задачи
Реализовать полноценные core типы и интерфейсы, заменив заглушки. IPlanner и IEnvironment — абстрактные интерфейсы; State и Path — конкретные структуры.

## Описание изменений

### Файл: `include/core/state.hpp`

**Структура State:**
- `double x, y` — координаты
- `std::optional<double> theta` — для SE(2)
- Для grid: `std::optional<std::pair<int,int>> grid_pos` — (row, col)
- Конструкторы, операторы сравнения (для использования в контейнерах)

### Файл: `include/core/path.hpp`

**Структура Path:**
- `std::vector<State> states`
- `bool success`
- `double length` — кэшированная длина пути (метод compute_length())
- Метод `bool empty() const`
- Метод `void compute_length()` — сумма расстояний между соседними states

### Файл: `include/core/iplanner.hpp`

**Класс IPlanner:**
```cpp
class IPlanner {
public:
    virtual Path solve(const IEnvironment& env, const State& start, const State& goal) = 0;
    virtual ~IPlanner() = default;
};
```

### Файл: `include/environment/ienvironment.hpp`

**Класс IEnvironment:**
```cpp
class IEnvironment {
public:
    virtual bool is_valid(const State& s) const = 0;
    virtual bool collision_free(const State& a, const State& b) const = 0;
    virtual double clearance(const State& s) const = 0;  // return 0.0 если не поддерживается
    virtual ~IEnvironment() = default;
};
```

### Интеграция
- Убедиться, что IEnvironment и IPlanner не создают циклических зависимостей (forward declaration при необходимости)
- core не зависит от environment (IPlanner принимает const IEnvironment&)

## Тест-кейсы

### Модульные тесты
1. **TC-UNIT-01:** Path::compute_length() для 3 точек на прямой
   - states: (0,0), (1,0), (2,0)
   - Ожидаемый результат: length == 2.0

2. **TC-UNIT-02:** Path::empty() и success
   - Пустой Path: empty()==true, success==false
   - Path с одной точкой: empty() по договорённости (можно считать не empty)

3. **TC-UNIT-03:** State сравнение
   - State(1,2) == State(1,2)
   - State(1,2) != State(1,3)

### Регрессионные тесты
- Все тесты из задачи 1.1 продолжают проходить

## Критерии приёмки
- [ ] State, Path реализованы полностью
- [ ] IPlanner, IEnvironment — чистые абстрактные интерфейсы
- [ ] Нет циклических зависимостей
- [ ] Все тесты проходят
