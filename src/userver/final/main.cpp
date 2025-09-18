#include <userver/utils/daemon_run.hpp>
#include "websocket.cpp"

using namespace userver;

/// [Websocket service sample - main]
int main(int argc, char* argv[]) {
    const auto component_list =
        components::MinimalServerComponentList().Append<samples::websocket::WebsocketsHandler>();
    return utils::DaemonMain(argc, argv, component_list);
}

/// [Websocket service sample - main]
