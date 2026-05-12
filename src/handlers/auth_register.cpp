#include "auth_register.hpp"

#include <userver/server/http/codes.hpp>
#include <userver/formats/json/exceptions.hpp>
#include <userver/utils/assert.hpp>

namespace handlers {

AuthRegister::AuthRegister(const userver::components::ComponentConfig& config,
                           const userver::components::ComponentContext& context)
    : BaseJson(config, context),
      storage_(context.FindComponent<storage::StorageComponent>()) {}

userver::formats::json::Value AuthRegister::HandleRequestJson(
    const userver::server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json) {

    if (!request_json.IsObject()) {
        request.SetResponseStatus(userver::server::http::Code::kBadRequest);
        return userver::formats::json::ValueBuilder().End();
    }

    std::string login, password, email, first_name, last_name;
    try {
        login = request_json["login"].As<std::string>();
        password = request_json["password"].As<std::string>();
        email = request_json["email"].As<std::string>();
        first_name = request_json["first_name"].As<std::string>();
        last_name = request_json["last_name"].As<std::string>();
    } catch (const userver::formats::json::MissingKeyException&) {
        request.SetResponseStatus(userver::server::http::Code::kBadRequest);
        return userver::formats::json::ValueBuilder()
            .Append("error", "Missing required fields: login, password, email, first_name, last_name")
            .End();
    }

    if (login.empty() || password.empty() || email.empty()) {
        request.SetResponseStatus(userver::server::http::Code::kBadRequest);
        return userver::formats::json::ValueBuilder()
            .Append("error", "Fields login, password, email cannot be empty")
            .End();
    }

    bool is_registered = storage_.GetStorage().RegisterUser(
        login, password, email, first_name, last_name
    );

    if (!is_registered) {
        request.SetResponseStatus(userver::server::http::Code::kConflict);
        return userver::formats::json::ValueBuilder()
            .Append("error", "User registration failed (login may already exist)")
            .End();
    }

    request.SetResponseStatus(userver::server::http::Code::kCreated);
    return userver::formats::json::ValueBuilder()
        .Append("status", "success")
        .Append("message", "User registered successfully")
        .Append("login", login)
        .End();
}

}