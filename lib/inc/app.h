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
        void receive();
        // 端口监听
        void scan();
        void udplisten();
        void tcplisten();
        void interpreter(const std::string& cmd);

    private:
        void                        respond(const ip::udp::endpoint& edp);
        void                        addEdp(const ip::tcp::endpoint& edp);
        std::vector<ip::address_v4> edpvec;  // 存储当前局域网扫描到的进程ip
        bool                        mstop;
        TaskPool                    mpool;
        std::thread                 mudplisten;
        std::thread                 mtcplisten;
    };
}  // namespace mtft

#endif  // MTFT_CONNECTION_H
