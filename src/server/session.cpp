//
// Created by 66 on 2026/6/21.
//

#include "session.h"

#include "../protocol/handshake.h"
#include "../utils/logger.h"

server::Session::Session(net::Socket sock, std::string peer_addr)
    : sock_(std::move(sock)),
      peer_addr_(std::move(peer_addr)) {
    utils::logger::info("[+] New client connected from %s", peer_addr_.c_str());
}

server::Session::~Session() {
    utils::logger::info("[-] Client %s disconnected", peer_addr_.c_str());
}

void server::Session::run() {
    protocol::Handshake handshake;
    char send_buf[4096]; // 存放 S0+S1+S2
    char recv_buf[4096]; // 存放 recv 到的原始数据

    while (handshake.state() != protocol::Handshake::State::DONE &&
           handshake.state() != protocol::Handshake::State::FAILED) {
        if (handshake.state() == protocol::Handshake::State::SEND_S0S1S2) {
            int len = handshake.get_send_data(send_buf, sizeof(send_buf));
            ::send(sock_.get(), send_buf, len, 0);
            handshake.advance_after_send();
            continue;
        }

        utils::logger::info("[DEBUG] State=%d, about to recv...",
                            static_cast<int>(handshake.state()));

        // ---- 接收数据 ----
        int n = ::recv(sock_.get(), recv_buf, sizeof(recv_buf), 0);
        if (n > 0) {
            // 把收到的数据喂给握手状态机
            int consumed = handshake.input(recv_buf, n);
            (void) consumed;
        } else if (n == 0) {
            utils::logger::error("[HANDSHAKE] Connection closed by peer during handshake");
            break;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
            break;
        }
    }

    if (handshake.state() == protocol::Handshake::State::DONE) {
        utils::logger::info("========================================");
        utils::logger::info("  ✓ RTMP Handshake Success!");
        utils::logger::info("========================================");
    }

    sock_.close();
}

void server::Session::print_hex(const char *data, int len) {
    constexpr int BYTES_PER_LINE = 16;
    for (int i = 0; i < len; i += BYTES_PER_LINE) {
        int remain = len - i;
        int count = (remain < BYTES_PER_LINE) ? remain : BYTES_PER_LINE;

        utils::logger::raw("  %04x  ", i);

        for (int j = 0; j < count; ++j) {
            utils::logger::raw("%02x ", static_cast<unsigned char>(data[i + j]));
        }

        for (int j = count; j < BYTES_PER_LINE; ++j) {
            utils::logger::raw("   ");
        }

        utils::logger::raw(" |");
        for (int j = 0; j < count; ++j) {
            auto c = static_cast<unsigned char>(data[i + j]);
            utils::logger::raw("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        utils::logger::raw("|\n");
    }
}
