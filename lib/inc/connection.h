//
// Created by xSeung on 2023/4/21.
//

#ifndef MAIN_CONNECTION_H
#define MAIN_CONNECTION_H
#define _WIN32_WINNT 0x0601
#include "asio.hpp"
#include "list"
#include "task.h"

namespace cncd {
        using namespace asio;
        class Connection {
            public:
                enum class State { CONNNECTED, DISCONNECTED };
                void setState(State state);

            private:
                ip::address_v4        m_host;   // 连接的对方主机ip
                State                 m_state;  // 连接状态
        };
}  // namespace cncd

#endif  // MAIN_CONNECTION_H
