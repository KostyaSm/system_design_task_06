#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace userver::storages::mongo { class Client; using ClientPtr = std::shared_ptr<Client>; }
namespace kafka { class ProducerComponent; }
namespace kafka::events {
    struct UserRegistered;
    struct WorkoutCreated;
    struct ExerciseAddedToWorkout;
}

namespace storage {

class Storage {
public:
    Storage(userver::storages::mongo::ClientPtr mongo_client,
            std::shared_ptr<kafka::ProducerComponent> kafka_producer);

    bool RegisterUser(const std::string& login, const std::string& password_hash,
                      const std::string& email, const std::string& first_name,
                      const std::string& last_name);

    bool CreateExercise(const std::string& name, const std::string& description,
                        const std::string& category, const std::string& difficulty);

    std::string CreateWorkout(const std::string& user_id, const std::string& name,
                              const std::string& date);
                              
    bool AddExerciseToWorkout(const std::string& workout_id, const std::string& exercise_id,
                              int sets, int reps, double weight_kg);

    void PublishUserRegistered(const kafka::events::UserRegistered& event);
    void PublishWorkoutCreated(const kafka::events::WorkoutCreated& event);
    void PublishExerciseAdded(const kafka::events::ExerciseAddedToWorkout& event);

private:
    userver::storages::mongo::ClientPtr mongo_client_;
    std::shared_ptr<kafka::ProducerComponent> kafka_producer_;

    void InsertUserRecord(const std::string& login, const std::string& password_hash,
                          const std::string& email, const std::string& first_name,
                          const std::string& last_name);
                          
    std::string GenerateObjectId() const;
};

}