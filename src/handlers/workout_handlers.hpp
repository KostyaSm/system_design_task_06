#pragma once

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

#include "storage/storage.hpp"

namespace handlers {

class CreateWorkout final : public server::handlers::HttpHandlerBase {
public:
    static constexpr auto kName = "handler-create-workout";
    CreateWorkout(const components::ComponentConfig& config, const components::ComponentContext& context);
    std::string HandleRequestThrow(const server::http::HttpRequest& request,
                                   server::request::RequestContext& context) const override;
private:
    storage::Storage& storage_;
};

class GetUserWorkouts final : public server::handlers::HttpHandlerBase {
public:
    static constexpr auto kName = "handler-get-user-workouts";
    GetUserWorkouts(const components::ComponentConfig& config, const components::ComponentContext& context);
    std::string HandleRequestThrow(const server::http::HttpRequest& request,
                                   server::request::RequestContext& context) const override;
private:
    storage::Storage& storage_;
};

class GetWorkoutStats final : public server::handlers::HttpHandlerBase {
public:
    static constexpr auto kName = "handler-get-workout-stats";
    GetWorkoutStats(const components::ComponentConfig& config, const components::ComponentContext& context);
    std::string HandleRequestThrow(const server::http::HttpRequest& request,
                                   server::request::RequestContext& context) const override;
private:
    storage::Storage& storage_;
};

}