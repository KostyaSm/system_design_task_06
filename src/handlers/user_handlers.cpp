#include "handlers/user_handlers.hpp"

#include <userver/server/http/http_status.hpp>
#include <userver/utils/json.hpp>

#include <fmt/format.h>

namespace handlers {

GetUserByLogin::GetUserByLogin(const components::ComponentConfig& config,
                               const components::ComponentContext& context)
    : HttpHandlerBase(config, context)
    , storage_(context.FindComponent<storage::Storage>()) {}

std::string GetUserByLogin::HandleRequestThrow(const server::http::HttpRequest& request,
                                               server::request::RequestContext& context) const {
    std::string login = std::string(request.GetArg("login"));
    if (login.empty()) {
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return utils::json::ToString(utils::json::MakeObject({"error", "login required"}));
    }

    std::string cache_key = fmt::format("user:login:{}", login);
    if (auto cached = storage_.GetCache(cache_key)) {
        return *cached;
    }

    auto user = storage_.GetUserByLogin(login);
    if (!user) {
        request.SetResponseStatus(server::http::HttpStatus::kNotFound);
        return utils::json::ToString(utils::json::MakeObject({"error", "User not found"}));
    }

    auto json = utils::json::MakeObject(
        {"id", user->id},
        {"login", user->login},
        {"first_name", user->first_name},
        {"last_name", user->last_name}
    );
    std::string response = utils::json::ToString(json);
    storage_.SetCache(cache_key, response, 900);
    return response;
}

FindUsersByMask::FindUsersByMask(const components::ComponentConfig& config,
                                 const components::ComponentContext& context)
    : HttpHandlerBase(config, context)
    , storage_(context.FindComponent<storage::Storage>()) {}

std::string FindUsersByMask::HandleRequestThrow(const server::http::HttpRequest& request,
                                                server::request::RequestContext& context) const {
    std::string mask = std::string(request.GetArg("mask"));
    if (mask.empty()) {
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return utils::json::ToString(utils::json::MakeObject({"error", "mask required"}));
    }

    auto users = storage_.FindUsersByMask(mask);
    auto json = utils::json::MakeArray();
    for (const auto& u : users) {
        json.PushBack(utils::json::MakeObject(
            {"id", u.id},
            {"login", u.login},
            {"first_name", u.first_name},
            {"last_name", u.last_name}
        ));
    }
    return utils::json::ToString(json);
}

RegisterUser::RegisterUser(const components::ComponentConfig& config,
                           const components::ComponentContext& context)
    : HttpHandlerBase(config, context)
    , storage_(context.FindComponent<storage::Storage>()) {}

std::string RegisterUser::HandleRequestThrow(const server::http::HttpRequest& request,
                                             server::request::RequestContext& context) const {
    auto json = utils::json::FromString(request.RequestBody());
    std::string login = json["login"].AsString();
    std::string first_name = json["first_name"].AsString();
    std::string last_name = json["last_name"].AsString();
    std::string password = json["password"].AsString();

    if (login.empty() || password.empty()) {
        request.SetResponseStatus(server::http::HttpStatus::kBadRequest);
        return utils::json::ToString(utils::json::MakeObject({"error", "login and password required"}));
    }

    if (!storage_.RegisterUser(login, first_name, last_name, password)) {
        request.SetResponseStatus(server::http::HttpStatus::kConflict);
        return utils::json::ToString(utils::json::MakeObject({"error", "User already exists"}));
    }

    return utils::json::ToString(utils::json::MakeObject({"status", "ok"}));
}

}