#include "storage.hpp"

#include <userver/storages/mongo/collection.hpp>
#include <userver/storages/mongo/operations.hpp>
#include <userver/utils/assert.hpp>

#include "kafka/events.hpp"
#include "kafka/component.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace storage {

using namespace userver::storages::mongo;

Storage::Storage(ClientPtr mongo_client,
                 std::shared_ptr<kafka::ProducerComponent> kafka_producer)
    : mongo_client_(std::move(mongo_client))
    , kafka_producer_(std::move(kafka_producer)) {}

bool Storage::RegisterUser(const std::string& login, const std::string& password_hash,
                           const std::string& email, const std::string& first_name,
                           const std::string& last_name) {
    try {
        InsertUserRecord(login, password_hash, email, first_name, last_name);
        
        kafka::events::UserRegistered event(
            GenerateObjectId(), login, email, first_name, last_name
        );
        PublishUserRegistered(event);
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Failed to register user: " << ex.what() << std::endl;
        return false;
    }
}

bool Storage::CreateExercise(const std::string& name, const std::string& description,
                             const std::string& category, const std::string& difficulty) {
    try {
        auto collection = mongo_client_->GetCollection("exercises");
        auto doc = MakeDoc()
            .Append("name", name)
            .Append("description", description)
            .Append("category", category)
            .Append("difficulty", difficulty)
            .Append("created_at", userver::utils::datetime::Now());
            
        auto result = collection.InsertOne(std::move(doc));
        UASSERT(result.GetInsertionResult().HasInsertedId());
        
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Failed to create exercise: " << ex.what() << std::endl;
        return false;
    }
}

std::string Storage::CreateWorkout(const std::string& user_id, const std::string& name,
                                   const std::string& date) {
    try {
        auto collection = mongo_client_->GetCollection("workouts");
        auto workout_id = GenerateObjectId();
        auto doc = MakeDoc()
            .Append("_id", ObjectId{workout_id})
            .Append("user_id", user_id)
            .Append("name", name)
            .Append("date", date)
            .Append("exercises", userver::formats::bson::MakeArray())
            .Append("created_at", userver::utils::datetime::Now());
            
        auto result = collection.InsertOne(std::move(doc));
        UASSERT(result.GetInsertionResult().HasInsertedId());
        
        kafka::events::WorkoutCreated event(
            workout_id, user_id, name, date, 0, 0
        );
        PublishWorkoutCreated(event);
        
        return workout_id;
    } catch (const std::exception& ex) {
        std::cerr << "Failed to create workout: " << ex.what() << std::endl;
        throw;
    }
}

bool Storage::AddExerciseToWorkout(const std::string& workout_id, const std::string& exercise_id,
                                   int sets, int reps, double weight_kg) {
    try {
        auto collection = mongo_client_->GetCollection("workouts");
        auto filter = MakeDoc().Append("_id", ObjectId{workout_id});
        auto exercise_doc = MakeDoc()
            .Append("exercise_id", exercise_id)
            .Append("sets", sets)
            .Append("reps", reps)
            .Append("weight_kg", weight_kg);
        auto update = MakeDoc().Append("$push", MakeDoc().Append("exercises", exercise_doc));
        
        auto result = collection.UpdateOne(std::move(filter), std::move(update));
        if (!result.IsModifiedCountAvailable() || result.GetModifiedCount() == 0) {
            return false;
        }
        
        kafka::events::ExerciseAddedToWorkout event(
            workout_id, exercise_id, sets, reps, weight_kg
        );
        PublishExerciseAdded(event);
        
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Failed to add exercise to workout: " << ex.what() << std::endl;
        return false;
    }
}

void Storage::PublishUserRegistered(const kafka::events::UserRegistered& event) {
    nlohmann::json json_event = {
        {"event_id", event.event_id},
        {"event_type", event.event_type},
        {"timestamp", event.timestamp},
        {"user_id", event.user_id},
        {"login", event.login},
        {"email", event.email},
        {"first_name", event.first_name},
        {"last_name", event.last_name}
    };
    kafka_producer_->GetProducer().Produce(
        "user-events", 
        event.user_id,
        json_event.dump()
    );
}

void Storage::PublishWorkoutCreated(const kafka::events::WorkoutCreated& event) {
    nlohmann::json json_event = {
        {"event_id", event.event_id},
        {"event_type", event.event_type},
        {"timestamp", event.timestamp},
        {"workout_id", event.workout_id},
        {"user_id", event.user_id},
        {"name", event.name},
        {"date", event.date},
        {"duration_minutes", event.duration_minutes},
        {"total_calories", event.total_calories}
    };
    kafka_producer_->GetProducer().Produce(
        "workout-events", 
        event.user_id,
        json_event.dump()
    );
}

void Storage::PublishExerciseAdded(const kafka::events::ExerciseAddedToWorkout& event) {
    nlohmann::json json_event = {
        {"event_id", event.event_id},
        {"event_type", event.event_type},
        {"timestamp", event.timestamp},
        {"workout_id", event.workout_id},
        {"exercise_id", event.exercise_id},
        {"sets", event.sets},
        {"reps", event.reps},
        {"weight_kg", event.weight_kg}
    };
    kafka_producer_->GetProducer().Produce(
        "workout-events", 
        event.workout_id,
        json_event.dump()
    );
}

void Storage::InsertUserRecord(const std::string& login, const std::string& password_hash,
                               const std::string& email, const std::string& first_name,
                               const std::string& last_name) {
    auto collection = mongo_client_->GetCollection("users");
    auto doc = MakeDoc()
        .Append("login", login)
        .Append("password_hash", password_hash)
        .Append("email", email)
        .Append("first_name", first_name)
        .Append("last_name", last_name)
        .Append("created_at", userver::utils::datetime::Now());
        
    auto result = collection.InsertOne(std::move(doc));
    UASSERT(result.GetInsertionResult().HasInsertedId());
}

std::string Storage::GenerateObjectId() const {
    return ObjectId{}.ToString();
}

}