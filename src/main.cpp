#include <iostream>

#include "server/server.h"

int main() {
    constexpr int RTMP_PORT = 1934;

    std::cout << "MiniRTMP starting on port: " << RTMP_PORT << '\n';
    server::Server server(RTMP_PORT);
    server.start();

    return 0;
}
