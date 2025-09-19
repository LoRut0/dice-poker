#include "websocket.hpp"

using namespace userver;

namespace services::websocket {
    
    formats::json::Value WebsocketsHandler::redisGetJson(std::string_view key) const {
        const std::optional<std::string> result = redis_client_->Get(std::string{key}, redis_cc_).Get();
        if (!result) {
            throw server::handlers::ConflictError(server::handlers::ExternalBody{"No game with key"});
            return {};
        }
        return formats::json::FromString(std::string_view(*result));
    }

    int WebsocketsHandler::redisPostJson(std::string_view key, const formats::json::Value &json_to_post) const {
        std::string jsonString{formats::json::ToStableString(json_to_post)};
        redis_client_->Set(std::string{key}, jsonString, redis_cc_).Get();
        return 201;
    }

    size_t WebsocketsHandler::redisDelete(std::string_view key) const {
        const size_t result = redis_client_->Del(std::string{key}, redis_cc_).Get();
        return result;
    }

}