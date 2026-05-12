# Fitness Tracker API (Event-Driven Architecture)

Проект реализует архитектуру фитнес-трекера с применением паттернов **Event-Driven** и **CQRS**, используя брокер сообщений **Apache Kafka**.

## Архитектура

Система разделена на два основных потока данных:

1. **Write Model (Command Side):**
   - Обработка входящих HTTP-запросов (регистрация, создание тренировок).
   - Сохранение данных в MongoDB.
   - Публикация событий (Events) в Kafka после успешной транзакции.

2. **Read Model (Query Side):**
   - Отдельный сервис-консьюмер (`statistics_consumer`), который слушает события из Kafka.
   - Агрегация данных и обновление аналитики (в будущем может писать в отдельную Read БД).

### Схема взаимодействия:
`Client` -> `API Handler` -> `Storage` -> `MongoDB (Write)` -> `Kafka Producer` -> `Kafka Broker` -> `Kafka Consumer` -> `Read Model Updates`

## Технологии

- **Язык:** C++20
- **Фреймворк:** userver (асинхронный сервер)
- **Брокер сообщений:** Apache Kafka (librdkafka)
- **База данных:** MongoDB 7
- **Кеш/Rate Limiting:** Redis 7
- **Контейнеризация:** Docker Compose

##  Структура проекта

- `src/` — Исходный код
  - `handlers/` — HTTP-обработчики (валидация и маршрутизация)
  - `storage/` — Слой бизнес-логики и доступа к данным (инкапсуляция MongoDB и Kafka)
  - `kafka/` — Реализация Producer и Consumer на базе librdkafka
- `configs/` — Конфигурация сервисов
- `event_driven_design.md` — Описание архитектуры
- `event_catalog.md` — Каталог событий (контракты JSON)

##  Запуск проекта

### 1. Сборка и запуск всех сервисов
Для запуска требуется установленный Docker и Docker Compose.

```bash
# Сборка образов и запуск контейнеров (Mongo, Redis, Kafka, API)
docker-compose up --build -d
```

Сервисы будут доступны по адресам:
- **API:** `http://localhost:8080`
- **Kafka UI:** `http://localhost:8090` (для просмотра сообщений)
- **MongoDB:** `localhost:27017`
- **Redis:** `localhost:6379`

### 2. Запуск Consumer (Обработчик событий)
Потребитель событий (`statistics_consumer`) реализован как отдельный процесс.
Вы можете запустить его локально (если собрана сборка) или добавить его в docker-compose.

**Локальный запуск:**
```bash
# Убедитесь, что Kafka доступна
./build/statistics_consumer "localhost:9092"
```
*Consumer будет выводить логи в консоль при получении новых событий.*

##  Тестирование API

### 1. Регистрация пользователя
```bash
curl -X POST http://localhost:8080/v1/auth/register \
     -H "Content-Type: application/json" \
     -d '{
           "login": "user1",
           "password": "secret",
           "email": "user1@test.com",
           "first_name": "John",
           "last_name": "Doe"
         }'
```
*Ожидаемый результат: HTTP 201. В логах Consumer должно появиться событие `UserRegistered`.*

### 2. Создание тренировки
```bash
curl -X POST http://localhost:8080/v1/workouts \
     -H "Content-Type: application/json" \
     -d '{
           "user_id": "507f191e810c19729de860ea",
           "name": "Morning Run",
           "date": "2023-10-25"
         }'
```
*Ожидаемый результат: HTTP 201, возвращает `workout_id`. В логах Consumer должно появиться событие `WorkoutCreated`.*

## Документация

- [Event-Driven Design](./event_driven_design.md) — Подробное описание архитектуры, выбор Kafka, гарантии доставки.
- [Event Catalog](./event_catalog.md) — Описание всех типов событий, их полей и схем JSON.

## Требования к сдаче
Проект выполнен в соответствии с заданием:
- [x] Анализ событий (Events & Commands)
- [x] Event-Driven архитектура (Producers/Consumers)
- [x] Брокер сообщений (Kafka, гарантии at-least-once)
- [x] Паттерн CQRS (Разделение Write/Read моделей)
- [x] Реализация Producer/Consumer
- [x] Каталог событий
