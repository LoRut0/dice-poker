#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/websocket/websocket_handler.hpp>

#include <string_view>

#define MAX_SEQ 20

using namespace userver;

namespace samples::websocket {

class WebsocketsHandler final : public server::websocket::WebsocketHandlerBase {
public:
    // `kName` is used as the component name in static config
    static constexpr std::string_view kName = "websocket-handler";

    // Component is valid after construction and is able to accept requests
    using WebsocketHandlerBase::WebsocketHandlerBase;

    void Handle(server::websocket::WebSocketConnection& chat, server::request::RequestContext&) const override {
        server::websocket::Message message;
        while (!engine::current_task::ShouldCancel()) {
            chat.Recv(message);               // throws on closed/dropped connection
            if (message.close_status) break;  // explicit close if any
            ParseMessage(message);
            chat.Send(std::move(message));    // throws on closed/dropped connection
        }
        if (message.close_status) chat.Close(*message.close_status);
    }
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
};

}

namespace samples::websocket {

    int WebsocketsHandler::JsonFormatCheck(formats::json::Value &msg) const {
        std::string action;
        action = msg["action"].ConvertTo<std::string>();
        if (action == "start") {
            return 2;
        }
        else if (action == "roll") {
            return 0;
        }
        else if (action == "choose") {
            return 1;
        }
        return -1;
    }

    void WebsocketsHandler::ParseMessage(server::websocket::Message &message) const {
        if (!message.is_text) {
            throw server::handlers::ClientError(server::handlers::ExternalBody{"No json in request"});
        }
        formats::json::Value json_msg{formats::json::FromString(std::string_view(message.data))};
        switch(JsonFormatCheck(json_msg)) {

        }

    }

}