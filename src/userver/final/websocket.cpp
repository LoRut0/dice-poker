#include "websocket.hpp"
#include <string_view>


#define MAX_SEQ 20

inline constexpr std::string_view OngoingCreateTable = R"~(
    CREATE TABLE IF NOT EXISTS ongoing (
    key INTEGER PRIMARY KEY,
    )
)~";

namespace services::websocket {
    
    WebsocketsHandler::WebsocketsHandler(const components::ComponentConfig& config, const components::ComponentContext& context) 
        : server::websocket::WebsocketHandlerBase(config, context),
          sqlite_client_(context.FindComponent<components::SQLite>("database").GetClient()),
          redis_client_{context.FindComponent<components::Redis>("key-json-database").GetClient("key-json")},
          redis_cc_{std::chrono::seconds{15}, std::chrono::seconds{60}, 4} 
    {

        sqlite_client_->Execute(storages::sqlite::OperationType::kReadWrite, OngoingCreateTable.data());
     }
    
    void WebsocketsHandler::Handle(server::websocket::WebSocketConnection& chat, server::request::RequestContext&) const {
        server::websocket::Message message;
        while (!engine::current_task::ShouldCancel()) {
            chat.Recv(message);               // throws on closed/dropped connection
            if (message.close_status) break;  // explicit close if any
            ParseMessage(message);
            chat.Send(std::move(message));    // throws on closed/dropped connection
        }
        if (message.close_status) chat.Close(*message.close_status);
    }

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
            case 2: {
                formats::json::ValueBuilder json_for_redis;
                int64_t gameID = sqlNewOngoing();
                DefaultInit(json_for_redis, gameID);
                
            }
            case 0: {
                //TODO: some
            }
        }

    }

    void WebsocketsHandler::DefaultInit(formats::json::ValueBuilder &json_for_redis, int64_t gameID) const {
        json_for_redis["gameID"] = gameID;
        //TODO: random queue at the beginning of game
        json_for_redis["state"]["playerq"] = 0;

        //p1
        json_for_redis["state"]["p1"]["dices"].Resize(5);
        for (int i = 0; i < 5; i++) {
            json_for_redis["state"]["p1"]["dices"][i] = -1;
        }
        json_for_redis["state"]["p1"]["rollnum"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["s1"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["s2"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["s3"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["s4"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["s5"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["s6"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["pair"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["twopair"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["bonus1"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["3kind"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["4kind"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["fullhouse"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["smstrght"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["lrgstrght"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["5kind"] = 0;
        json_for_redis["state"]["p1"]["sequences"]["chance"] = 0;

        //p2
        json_for_redis["state"]["p2"]["dices"].Resize(5);
        for (int i = 0; i < 5; i++) {
            json_for_redis["state"]["p2"]["dices"][i] = -1;
        }
        json_for_redis["state"]["p2"]["rollnum"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["s1"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["s2"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["s3"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["s4"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["s5"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["s6"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["pair"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["twopair"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["bonus1"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["3kind"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["4kind"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["fullhouse"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["smstrght"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["lrgstrght"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["5kind"] = 0;
        json_for_redis["state"]["p2"]["sequences"]["chance"] = 0;
        
        return;
    }

}

