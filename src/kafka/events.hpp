#pragma once

#include <string>
#include <chrono>
#include <fmt/format.h>

namespace kafka::events {

inline std::string GenerateTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    return fmt::format("{}", time_t_now);
}

inline std::string GenerateEventId() {
    auto now = std::chrono::steady_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    return fmt::format("evt_{}", timestamp);
}

struct UserRegistered {
    std::string event_id;
    std::string event_type = "UserRegistered";
    std::string timestamp;
    std::string user_id;
    std::string login;
    std::string email;
    std::string first_name;
    std::string last_name;
    
    UserRegistered() : timestamp(GenerateTimestamp()), event_id(GenerateEventId()) {}
    
    UserRegistered(const std::string& uid, const std::string& log, 
                   const std::string& eml, const std::string& fname, 
                   const std::string& lname)
        : timestamp(GenerateTimestamp()), event_id(GenerateEventId()),
          user_id(uid), login(log), email(eml), 
          first_name(fname), last_name(lname) {}
};

struct UserUpdated {
    std::string event_id;
    std::string event_type = "UserUpdated";
    std::string timestamp;
    std::string user_id;
    std::string updated_fields;  // JSON string with changed fields
    
    UserUpdated() : timestamp(GenerateTimestamp()), event_id(GenerateEventId()) {}
    
    UserUpdated(const std::string& uid, const std::string& fields)
        : timestamp(GenerateTimestamp()), event_id(GenerateEventId()),
          user_id(uid), updated_fields(fields) {}
};

struct ExerciseCreated {
    std::string event_id;
    std::string event_type = "ExerciseCreated";
    std::string timestamp;
    std::string exercise_id;
    std::string name;
    std::string description;
    std::string category;
    std::string difficulty;
    
    ExerciseCreated() : timestamp(GenerateTimestamp()), event_id(GenerateEventId()) {}
    
    ExerciseCreated(const std::string& eid, const std::string& nm, 
                    const std::string& desc, const std::string& cat, 
                    const std::string& diff)
        : timestamp(GenerateTimestamp()), event_id(GenerateEventId()),
          exercise_id(eid), name(nm), description(desc), 
          category(cat), difficulty(diff) {}
};

struct ExerciseUpdated {
    std::string event_id;
    std::string event_type = "ExerciseUpdated";
    std::string timestamp;
    std::string exercise_id;
    std::string updated_fields;
    
    ExerciseUpdated() : timestamp(GenerateTimestamp()), event_id(GenerateEventId()) {}
    
    ExerciseUpdated(const std::string& eid, const std::string& fields)
        : timestamp(GenerateTimestamp()), event_id(GenerateEventId()),
          exercise_id(eid), updated_fields(fields) {}
};

struct WorkoutCreated {
    std::string event_id;
    std::string event_type = "WorkoutCreated";
    std::string timestamp;
    std::string workout_id;
    std::string user_id;
    std::string name;
    std::string date;
    int duration_minutes;
    int total_calories;
    
    WorkoutCreated() : timestamp(GenerateTimestamp()), event_id(GenerateEventId()),
                       duration_minutes(0), total_calories(0) {}
    
    WorkoutCreated(const std::string& wid, const std::string& uid, 
                   const std::string& nm, const std::string& dt,
                   int duration, int calories)
        : timestamp(GenerateTimestamp()), event_id(GenerateEventId()),
          workout_id(wid), user_id(uid), name(nm), date(dt),
          duration_minutes(duration), total_calories(calories) {}
};

struct ExerciseAddedToWorkout {
    std::string event_id;
    std::string event_type = "ExerciseAddedToWorkout";
    std::string timestamp;
    std::string workout_id;
    std::string exercise_id;
    int sets;
    int reps;
    double weight_kg;
    
    ExerciseAddedToWorkout() : timestamp(GenerateTimestamp()), event_id(GenerateEventId()),
                                sets(0), reps(0), weight_kg(0.0) {}
    
    ExerciseAddedToWorkout(const std::string& wid, const std::string& eid,
                           int s, int r, double w)
        : timestamp(GenerateTimestamp()), event_id(GenerateEventId()),
          workout_id(wid), exercise_id(eid), sets(s), reps(r), weight_kg(w) {}
};

struct WorkoutCompleted {
    std::string event_id;
    std::string event_type = "WorkoutCompleted";
    std::string timestamp;
    std::string workout_id;
    std::string user_id;
    int total_duration_minutes;
    int total_calories_burned;
    int exercises_count;
    
    WorkoutCompleted() : timestamp(GenerateTimestamp()), event_id(GenerateEventId()),
                         total_duration_minutes(0), total_calories_burned(0),
                         exercises_count(0) {}
    
    WorkoutCompleted(const std::string& wid, const std::string& uid,
                     int duration, int calories, int exercises)
        : timestamp(GenerateTimestamp()), event_id(GenerateEventId()),
          workout_id(wid), user_id(uid), total_duration_minutes(duration),
          total_calories_burned(calories), exercises_count(exercises) {}
};

}