#include "create_workout.hpp"

#include <userver/server/http/codes.hpp>
#include <userver/formats/json/exceptions.hpp>
#include <userver/utils/assert.hpp>

namespace handlers {

CreateWorkout::CreateWorkout(const userver::components::ComponentConfig& config,
                             const userver::components::ComponentContext& context)
    : BaseJson(config, context),
      storage_(context.FindComponent<storage::StorageComponent>()) {}

userver::formats::json::Value CreateWorkout::HandleRequestJson(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json) {

    if (!request_json.IsObject()) {
        request.SetResponseStatus(userver::server::http::Code::kBadRequest);
        return userver::formats::json::ValueBuilder()
            .Append("error", "Request body must be a JSON object")
            .End();
    }
    
    std::string user_id, name, date;
    try {
        user_id = request_json["user_id"].As<std::string>();
        name = request_json["name"].As<std::string>();
        date = request_json["date"].As<std::string>(); // Формат YYYY-MM-DD ожидается
    } catch (const userver::formats::json::MissingKeyException& ex) {
        request.SetResponseStatus(userver::server::http::Code::kBadRequest);
        return userver::formats::json::ValueBuilder()
            .Append("error", fmt::format("Missing required field: {}", ex.key))
            .End();
    }

    try {
        std::string workout_id = storage_.GetStorage().CreateWorkout(user_id, name, date);
        
        request.SetResponseStatus(userver::server::http::Code::kCreated);
        return userver::formats::json::ValueBuilder()
            .Append("status", "success")
            .Append("workout_id", workout_id)
            .End();

    } catch (const std::exception& ex) {
        std::cerr << "Error creating workout: " << ex.what() << std::endl;
        
        request.SetResponseStatus(userver::server::http::Code::kInternalServerError);
        return userver::formats::json::ValueBuilder()
            .Append("error", "Internal server error while creating workout")
            .End();
    }
}

}