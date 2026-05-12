#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/components/component_context.hpp>
#include "storage/storage_component.hpp"

namespace handlers {

class CreateWorkout final : public userver::server::handlers::BaseJson {
public:
    static constexpr auto kName = "handler-create-workout";

    CreateWorkout(const userver::components::ComponentConfig& config,
                  const userver::components::ComponentContext& context);

    userver::formats::json::Value HandleRequestJson(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json) override;

private:
    storage::StorageComponent& storage_;
};

}