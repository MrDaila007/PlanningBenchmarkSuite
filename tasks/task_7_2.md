# Задача 7.2: CI pipeline, Docker, reproducibility

## Связь с юзер-кейсами
- UC-07: Репродукция экспериментов
- UC-10: Тестирование и CI

## Цель задачи
Настроить CI (GitHub Actions или GitLab CI), Docker-образ, обеспечить воспроизводимость и performance regression проверку.

## Описание изменений

### CI Pipeline (GitHub Actions / .github/workflows/)

**Этапы:**
1. Checkout
2. Setup: установка vcpkg/conan, зависимостей
3. Build: cmake, make
4. Unit + Integration tests
5. Performance tests (опционально, с baseline)
6. Doxygen build (если есть docs)
7. (Опционально) Performance regression: сравнение с baseline, fail если >10% деградация

### Docker

**Файлы:**
- `docker/Dockerfile`
- `docker/docker-compose.yml` (опционально)

**Dockerfile:**
- Base: ubuntu:22.04 или аналогичный
- Установка: build-essential, cmake, vcpkg, зависимости
- COPY проекта
- Build библиотеки и benchmark
- Запуск тестов
- Entrypoint: ./benchmark или /bin/bash для интерактивного использования

**Reproducibility:**
- Фиксированные версии зависимостей в Dockerfile
- Пример: запуск с config.json внутри контейнера даёт воспроизводимые результаты при том же seed

### Performance regression
- Сохранять baseline (например, mean time для 100x100 A*) в репо или CI artifacts
- При каждом PR: запустить performance test, сравнить с baseline
- При деградации > 10%: warning или fail (конфигурируемо)

### Сравнение коммитов
- Скрипт или CI job: запустить benchmark на двух коммитах, сравнить результаты
- Документация в README

## Тест-кейсы

### End-to-end
1. **TC-E2E-01:** Docker build и test
   - `docker build -t pbs . && docker run pbs ctest`
   - Exit 0

2. **TC-E2E-02:** Воспроизводимость в Docker
   - Запуск benchmark с config дважды
   - Результаты идентичны при том же seed

### CI
3. Push в репо → CI запускается, все этапы проходят
4. При умышленной поломке теста → CI fail

## Критерии приёмки
- [ ] CI pipeline настроен (GitHub Actions или GitLab CI)
- [ ] Docker образ собирается
- [ ] Тесты прогоняются в CI
- [ ] Performance regression проверка (опционально)
- [ ] README обновлён: как воспроизвести эксперименты
- [ ] Все тесты проходят
