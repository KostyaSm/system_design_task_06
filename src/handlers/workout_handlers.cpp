#include "handlers/workout_handlers.hpp"

#include <userver/server/http/http_status.hpp>
#include <userver/utils/json.hpp>

#include <fmt/format.h>

namespace handlers {

CreateWorkout::CreateWorkout(const components::ComponentConfig& config,
                             const components::ComponentContext& context)
    : HttpHandlerBase(config, context)
    , storage_(context.FindComponent<storage::Storage>()) {}

std::string CreateWorkout::HandleRequestThrow(const server::http::HttpRequest& request,
                                              server::request::RequestContext& context) const {
    std::string user_id = std::string(request.GetArg("user_id"));
    if (user_id.empty()) {
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return utils::json::ToString(utils::json::MakeObject({"error", "user_id required"}));
    }

    std::string rate_key = fmt::format("rate:create_workout:{}", user_id);
    if (!storage_.CheckRateLimit(rate_key, 100, 60)) {
        request.SetResponseStatus(server::http::HttpStatus::kTooManyRequests);
        request.SetHeader("X-RateLimit-Limit", "100");
        request.SetHeader("X-RateLimit-Remaining", "0");
        request.SetHeader("X-RateLimit-Reset", "60");
        return utils::json::ToString(utils::json::MakeObject({"error", "Rate limit exceeded"}));
    }

    auto json = utils::json::FromString(request.RequestBody());
    std::string date = json["date"].AsString();
    if (date.empty()) {
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return utils::json::ToString(utils::json::MakeObject({"error", "date required"}));
    }

    if (!storage_.CreateWorkout(user_id, date)) {
        request.SetResponseStatus(server::http::HttpStatus::kInternalServerError);
        return utils::json::ToString(utils::json::MakeObject({"error", "Failed to create workout"}));
    }

    return utils::json::ToString(utils::json::MakeObject({"status", "created"}));
}

GetUserWorkouts::GetUserWorkouts(const components::ComponentConfig& config,
                                 const components::ComponentContext& context)
    : HttpHandlerBase(config, context)
    , storage_(context.FindComponent<storage::Storage>()) {}

std::string GetUserWorkouts::HandleRequestThrow(const server::http::HttpRequest& request,
                                                server::request::RequestContext& context) const {
    std::string user_id = std::string(request.GetArg("user_id"));
    if (user_id.empty()) {
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return utils::json::ToString(utils::json::MakeObject({"error", "user_id required"}));
    }

    std::string cache_key = fmt::format("workouts:user:{}", user_id);
    if (auto cached = storage_.GetCache(cache_key)) {
        return *cached;
    }

    auto workouts = storage_.GetUserWorkouts(user_id);
    auto json = utils::json::MakeArray();
    for (const auto& w : workouts) {
        json.PushBack(utils::json::MakeObject(
            {"id", w.id},
            {"user_id", w.user_id},
            {"date", w.date},
            {"exercise_count", static_cast<int>(w.exercise_ids.size())}
        ));
    }

    std::string response = utils::json::ToString(json);
    storage_.SetCache(cache_key, response, 300);
    return response;
}

GetWorkoutStats::GetWorkoutStats(const components::ComponentConfig& config,
                                 const components::ComponentContext& context)
    : HttpHandlerBase(config, context)
    , storage_(context.FindComponent<storage::Storage>()) {}

std::string GetWorkoutStats::HandleRequestThrow(const server::http::HttpRequest& request,
                                                server::request::RequestContext& context) const {
    std::string user_id = std::string(request.GetArg("user_id"));
    std::string start_date = std::string(request.GetArg("start_date", ""));
    std::string end_date = std::string(request.GetArg("end_date", ""));

    if (user_id.empty()) {
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return utils::json::ToString(utils::json::MakeObject({"error", "user_id required"}));
    }

    std::string cache_key = fmt::format("stats:{}:{}:{}", user_id, start_date, end_date);
    if (auto cached = storage_.GetCache(cache_key)) {
        return *cached;
    }

    auto workouts = storage_.GetUserWorkouts(user_id);
    int total_workouts = 0;
    int total_exercises = 0;

    for (const auto& w : workouts) {
        bool in_range = true;
        if (!start_date.empty() && w.date < start_date) in_range = false;
        if (!end_date.empty() && w.date > end_date) in_range = false;
        if (in_range) {
            total_workouts++;
            total_exercises += w.exercise_ids.size();
        }
    }

    auto json = utils::json::MakeObject(
        {"total_workouts", total_workouts},
        {"total_exercises", total_exercises}
    );

    std::string response = utils::json::ToString(json);
    storage_.SetCache(cache_key, response, 300);
    return response;
}

}