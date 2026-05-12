#pragma once

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

#include "storage/storage.hpp"

namespace handlers {

class GetExercises final : public server::handlers::HttpHandlerBase {
public:
    static constexpr auto kName = "handler-get-exercises";
    
    GetExercises(const components::ComponentConfig& config, const components::ComponentContext& context);
    
    std::string HandleRequestThrow(const server::http::HttpRequest& request,
                                   server::request::RequestContext& context) const override;
    
private:
    storage::Storage& storage_;
};

class CreateExercise final : public server::handlers::HttpHandlerBase {
public:
    static constexpr auto kName = "handler-create-exercise";
    
    CreateExercise(const components::ComponentConfig& config, const components::ComponentContext& context);
    
    std::string HandleRequestThrow(const server::http::HttpRequest& request,
                                   server::request::RequestContext& context) const override;
    
private:
    storage::Storage& storage_;
};

}