#include "handlers/exercise_handlers.hpp"

#include <userver/server/http/http_status.hpp>
#include <userver/utils/json.hpp>

#include <fmt/format.h>

namespace handlers {

GetExercises::GetExercises(const components::ComponentConfig& config,
                           const components::ComponentContext& context)
    : HttpHandlerBase(config, context)
    , storage_(context.FindComponent<storage::Storage>()) {}

std::string GetExercises::HandleRequestThrow(const server::http::HttpRequest& request,
                                             server::request::RequestContext& context) const {
    const auto& params = request.GetArgs();
    
    std::string category = params.Get("category", "");
    int difficulty = std::stoi(params.Get("difficulty", "0"));
    
    auto exercises = storage_.GetExercises(category, difficulty);
    
    auto json = utils::json::MakeArray();
    for (const auto& ex : exercises) {
        json.PushBack(utils::json::MakeObject(
            {"id", ex.id},
            {"name", ex.name},
            {"category", ex.category},
            {"difficulty", ex.difficulty}
        ));
    }
    
    return utils::json::ToString(json);
}

CreateExercise::CreateExercise(const components::ComponentConfig& config,
                               const components::ComponentContext& context)
    : HttpHandlerBase(config, context)
    , storage_(context.FindComponent<storage::Storage>()) {}

std::string CreateExercise::HandleRequestThrow(const server::http::HttpRequest& request,
                                               server::request::RequestContext& context) const {
    const auto& body = request.RequestBody();
    auto json = utils::json::FromString(body);
    
    std::string name = json["name"].AsString();
    std::string category = json["category"].AsString();
    int difficulty = json["difficulty"].AsInt32();
    
    if (name.empty() || category.empty()) {
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return utils::json::ToString(utils::json::MakeObject({"error", "Invalid input"}));
    }
    
    bool created = storage_.CreateExercise(name, category, difficulty);
    if (!created) {
        request.SetResponseStatus(server::http::HttpStatus::kConflict);
        return utils::json::ToString(utils::json::MakeObject({"error", "Exercise already exists"}));
    }
    
    return utils::json::ToString(utils::json::MakeObject({"status", "created"}));
}

}