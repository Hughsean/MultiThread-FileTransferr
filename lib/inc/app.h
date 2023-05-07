//
// Created by xSeung on 2023/4/21.
//

#ifndef MTFT_APP_H
#define MTFT_APP_H
#define _WIN32_WINNT 0x0601

#include "asio.hpp"
#include "task.h"

namespace mtft {
    using namespace asio;
    class App {
    public:
        App(std::string rIP);
        void send(std::string fPath);
        void receive();
        void scan();
        void connect();

    private:
        void                        listen();
        void                        respond();
        void                        addEdp(const ip::tcp::endpoint& edp);
        std::vector<ip::address_v4> edpvec;  // 存储当前局域网扫描到的进程ip
        std::string                 path;    // 接收文件块存储位置
        bool                        mstop;
    };
}  // namespace mtft

#endif  // MTFT_CONNECTION_H
