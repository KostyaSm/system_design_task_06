#pragma once

#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>

#include "storage/storage.hpp"

namespace handlers {

class GetUserByLogin final : public server::handlers::HttpHandlerBase {
public:
    static constexpr auto kName = "handler-get-user-by-login";
    GetUserByLogin(const components::ComponentConfig& config, const components::ComponentContext& context);
    std::string HandleRequestThrow(const server::http::HttpRequest& request,
                                   server::request::RequestContext& context) const override;
private:
    storage::Storage& storage_;
};

class FindUsersByMask final : public server::handlers::HttpHandlerBase {
public:
    static constexpr auto kName = "handler-find-users-by-mask";
    FindUsersByMask(const components::ComponentConfig& config, const components::ComponentContext& context);
    std::string HandleRequestThrow(const server::http::HttpRequest& request,
                                   server::request::RequestContext& context) const override;
private:
    storage::Storage& storage_;
};

class RegisterUser final : public server::handlers::HttpHandlerBase {
public:
    static constexpr auto kName = "handler-register-user";
    RegisterUser(const components::ComponentConfig& config, const components::ComponentContext& context);
    std::string HandleRequestThrow(const server::http::HttpRequest& request,
                                   server::request::RequestContext& context) const override;
private:
    storage::Storage& storage_;
};

}