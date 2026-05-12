
# Event Catalog - Fitness Tracker

---

## 1. UserRegistered

### Описание
Событие генерируется при успешной регистрации нового пользователя в системе.

### Структура Payload (JSON)
```json
{
  "event_id": "evt_1715623849123",
  "event_type": "UserRegistered",
  "timestamp": "1715623849",
  "user_id": "507f191e810c19729de860ea",
  "login": "john_doe",
  "email": "john@example.com",
  "first_name": "John",
  "last_name": "Doe"
}
```

### Поля
| Поле | Тип | Описание |
|------|-----|----------|
| `event_id` | string | Уникальный идентификатор события |
| `event_type` | string | Тип события ("UserRegistered") |
| `timestamp` | string | Unix timestamp создания события |
| `user_id` | string | MongoDB ObjectId нового пользователя |
| `login` | string | Логин пользователя |
| `email` | string | Email пользователя |
| `first_name` | string | Имя |
| `last_name` | string | Фамилия |

### Метаданные
- **Топик Kafka:** `user-events`
- **Key:** `user_id` (для упорядочивания событий по пользователю)
- **Producer:** `AuthRegister Handler` -> `Storage`
- **Consumers:** 
  - `Statistics Service` (обновляет список активных пользователей)
  - `Notification Service` (отправляет приветственное письмо)
- **Delivery Guarantee:** `at-least-once`

---

## 2. WorkoutCreated

### Описание
Событие генерируется, когда пользователь создает новую пустую тренировку.

### Структура Payload (JSON)
```json
{
  "event_id": "evt_1715623900456",
  "event_type": "WorkoutCreated",
  "timestamp": "1715623900",
  "workout_id": "507f191e810c19729de860eb",
  "user_id": "507f191e810c19729de860ea",
  "name": "Morning Run",
  "date": "2023-05-15",
  "duration_minutes": 0,
  "total_calories": 0
}
```

### Поля
| Поле | Тип | Описание |
|------|-----|----------|
| `workout_id` | string | Идентификатор тренировки |
| `user_id` | string | Идентификатор владельца тренировки |
| `name` | string | Название тренировки |
| `date` | string | Дата проведения (YYYY-MM-DD) |
| `duration_minutes` | int | Начальное значение (0) |
| `total_calories` | int | Начальное значение (0) |

### Метаданные
- **Топик Kafka:** `workout-events`
- **Key:** `user_id`
- **Producer:** `CreateWorkout Handler` -> `Storage`
- **Consumers:** 
  - `Statistics Service` (создает запись в таблице статистики пользователя)
- **Delivery Guarantee:** `at-least-once`

---

## 3. ExerciseAddedToWorkout

### Описание
Событие генерируется при добавлении конкретного упражнения в существующую тренировку.

### Структура Payload (JSON)
```json
{
  "event_id": "evt_1715624000789",
  "event_type": "ExerciseAddedToWorkout",
  "timestamp": "1715624000",
  "workout_id": "507f191e810c19729de860eb",
  "exercise_id": "507f191e810c19729de860ec",
  "sets": 3,
  "reps": 10,
  "weight_kg": 50.5
}
```

### Поля
| Поле | Тип | Описание |
|------|-----|----------|
| `workout_id` | string | Идентификатор тренировки |
| `exercise_id` | string | Идентификатор упражнения |
| `sets` | int | Количество подходов |
| `reps` | int | Количество повторений |
| `weight_kg` | double | Вес отягощения |

### Метаданные
- **Топик Kafka:** `workout-events`
- **Key:** `workout_id`
- **Producer:** `AddExerciseToWorkout Handler` -> `Storage`
- **Consumers:** 
  - `Statistics Service` (пересчитывает общую нагрузку тренировки)
- **Delivery Guarantee:** `at-least-once`
