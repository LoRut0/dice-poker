#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/websocket/websocket_handler.hpp>

#include <userver/storages/sqlite/client.hpp>
#include <userver/storages/sqlite/component.hpp>
#include <userver/storages/sqlite/operation_types.hpp>

#include <userver/storages/redis/client.hpp>
#include <userver/storages/redis/component.hpp>

//?
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>

using namespace userver;

namespace services::websocket {

class WebsocketsHandler final : public server::websocket::WebsocketHandlerBase {
public:
    // `kName` is used as the component name in static config
    static constexpr std::string_view kName = "websocket-handler";
    
    WebsocketsHandler(const components::ComponentConfig& config, const components::ComponentContext& context);

    // Component is valid after construction and is able to accept requests
    using WebsocketHandlerBase::WebsocketHandlerBase;

    void Handle(server::websocket::WebSocketConnection& chat, server::request::RequestContext&) const override;

private:
    /*
     * @brief Parses message from websocket, calls functions
     *
     * @throws {server::handlers::ClientError} if format of msg is bad
     */
    void ParseMessage(server::websocket::Message &message) const;
    /*
     * @brief checks if Json Format corresponds with expected
     *
     * @returns {0 | 1 | 2} 0-roll, 1-choose, 2-start
     * @returns {-1} If format is bad
     */
    int JsonFormatCheck(formats::json::Value &msg) const;
    
    void DefaultInit(formats::json::ValueBuilder &json_for_redis, int64_t gameID) const;
    

    /// [SQLITE component]
    
    std::vector<int> sqlGetAllOngoing(std::string_view key, const server::websocket::Message &message) const;
    int64_t sqlNewOngoing() const;
    int64_t sqlDeleteValue(std::string_view key) const;

    storages::sqlite::ClientPtr sqlite_client_;
    
    /// [REDIS component]
    
    formats::json::Value WebsocketsHandler::redisGetJson(std::string_view key) const;
    int redisPostJson(std::string_view key, const formats::json::Value &json_to_post) const;
    size_t redisDelete(std::string_view key) const;
    
    storages::redis::ClientPtr redis_client_;
    storages::redis::CommandControl redis_cc_;
};

}