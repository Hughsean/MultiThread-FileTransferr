/**
 * @brief 应用模块
 *
 * @version 0.1
 * @author 鳯玖 (xSeung@163.com)
 * @date 2023-04-21
 * @copyright Copyright (c) 2023
 */

#ifndef MTFT_APP_H
#define MTFT_APP_H
#define _WIN32_WINNT 0x0601

#include "asio.hpp"
#include "task.h"

namespace mtft {
    using namespace asio;
    class App {
    public:
        App();
        ~App();
        void send(const std::string& fPath, const ip::address_v4& ip);
        void scan();
        void interpreter(const std::string& cmd);

    private:
        // 端口监听
        void        udplisten();
        void        tcplisten();
        void        respond(const ip::udp::endpoint& edp);
        bool        mstop;
        TaskPool    mpool;
        std::thread mudplisten;
        std::thread mtcplisten;
    };
}  // namespace mtft

#endif  // MTFT_CONNECTION_H
