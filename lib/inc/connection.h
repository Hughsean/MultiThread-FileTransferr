//
// Created by xSeung on 2023/4/21.
//

#ifndef MAIN_CONNECTION_H
#define MAIN_CONNECTION_H
#define _WIN32_WINNT 0x0601
#include "asio.hpp"
#include "list"
#include "task.h"

namespace mtft {
    using namespace asio;
    class Connection {
    public:
        Connection();

    private:
        ip::tcp::endpoint medp;  // 对方主机
    };
}  // namespace mtft

#endif  // MAIN_CONNECTION_H
