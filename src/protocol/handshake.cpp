//
// Created by 66 on 2026/6/21.
//

#include "handshake.h"

namespace protocol {
    Handshake::Handshake() {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    int Handshake::input(const char *data, int len) {
        int consumed = 0;
        if (state_ == State::WAIT_C0C1) {
            int need = 1537 - recv_buf_idx_;
            int take = (len < need) ? len : need;

            std::memcpy(recv_buf_ + recv_buf_idx_, data, take);
            recv_buf_idx_ += take;
            consumed = take;

            if (recv_buf_idx_ == 1537) {
                parse_c0c1();
            }
        } else if (state_ == State::WAIT_C2) {
            int need = 1536 - recv_buf_idx_;
            int take = (len < need) ? len : need;

            std::memcpy(recv_buf_ + recv_buf_idx_, data, take);
            recv_buf_idx_ += take;
            consumed = take;

            if (recv_buf_idx_ == 1536) {
                utils::logger::info("[HANDSHAKE] Received C2 (%d bytes)", recv_buf_idx_);
                state_ = State::DONE;
            }
        }
        return consumed;
    }

    int Handshake::get_send_data(char *out, int capacity) {
        // S0(1) + S1(1536) + S2(1536) = 3073 字节
        if (capacity < 3073) return 0;

        int idx = 0;
        // ============ S0: 1 字节 ============
        out[idx++] = 0x003; // RTMP 版本号 3
        // ============ S1: 1536 字节 ============
        uint32_t s1_ts = static_cast<uint32_t>(std::time(nullptr));
        // S1 timestamp（大端 4 字节）
        out[idx++] = static_cast<char>((s1_ts >> 24) & 0xFF);
        out[idx++] = static_cast<char>((s1_ts >> 16) & 0xFF);
        out[idx++] = static_cast<char>((s1_ts >> 8) & 0xFF);
        out[idx++] = static_cast<char>(s1_ts & 0xFF);

        // S1 zero（4 字节，全 0）
        out[idx++] = 0;
        out[idx++] = 0;
        out[idx++] = 0;
        out[idx++] = 0;

        // S1 random（1528 字节，随机填充）
        for (int i = 0; i < 1528; ++i) {
            out[idx++] = static_cast<char>(std::rand() & 0xFF);
        }

        // ============ S2: 1536 字节（回显 C1） ============
        // S2[0..3] = C1 timestamp（原样回显）
        out[idx++] = static_cast<char>((c1_timestamp_ >> 24) & 0xFF);
        out[idx++] = static_cast<char>((c1_timestamp_ >> 16) & 0xFF);
        out[idx++] = static_cast<char>((c1_timestamp_ >> 8) & 0xFF);
        out[idx++] = static_cast<char>(c1_timestamp_ & 0xFF);

        // S2[4..7] = 我们自己的 S1 timestamp（告知客户端我们的时间）
        out[idx++] = static_cast<char>((s1_ts >> 24) & 0xFF);
        out[idx++] = static_cast<char>((s1_ts >> 16) & 0xFF);
        out[idx++] = static_cast<char>((s1_ts >> 8) & 0xFF);
        out[idx++] = static_cast<char>(s1_ts & 0xFF);

        // S2[8..1535] = 原样回显 C1 的 random data（1528 字节）
        // C1 random 在 recv_buf_ 中偏移 9 字节处
        std::memcpy(out + idx, recv_buf_ + 9, 1528);
        idx += 1528;

        utils::logger::info(
            "[HANDSHAKE] Built S0+S1+S2: S1 timestamp=%u, total=%d bytes",
            s1_ts, idx);

        return idx; // 总是 3073
    }

    void Handshake::advance_after_send() {
        if (state_ == State::SEND_S0S1S2) {
            state_ == State::WAIT_C2;
            recv_buf_idx_ = 0;
            utils::logger::info("[HANDSHAKE] Sent S0+S1+S2, now waiting for C2...");
        }
    }

    void Handshake::parse_c0c1() {
        // recv_buf_ 中的布局：
        //   [0]      = C0 (1 字节版本号)
        //   [1..4]   = C1 timestamp (4 字节，大端)
        //   [5..8]   = C1 zero (4 字节，全 0)
        //   [9..1536]= C1 random data (1528 字节)
        uint8_t version = static_cast<uint8_t>(recv_buf_[0]);
        utils::logger::info("[HANDSHAKE] Received C0: version=%d", version);

        if (version != 3) {
            utils::logger::error("[HANDSHAKE] Unsupported RTMP version: %d", version);
            state_ = State::FAILED;
            return;
        }

        c1_timestamp_ =
                (static_cast<uint32_t>(static_cast<uint8_t>(recv_buf_[1])) << 24) |
                (static_cast<uint32_t>(static_cast<uint8_t>(recv_buf_[2])) << 16) |
                (static_cast<uint32_t>(static_cast<uint8_t>(recv_buf_[3])) << 8) |
                (static_cast<uint32_t>(static_cast<uint8_t>(recv_buf_[4])));

        utils::logger::info("[HANDSHAKE] Received C1: timestamp=%u, total=%d bytes",
                            c1_timestamp_, recv_buf_idx_);

        state_ = State::SEND_S0S1S2;
    }
}
