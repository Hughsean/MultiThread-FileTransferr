//
// Created by xSeung on 2023/4/20.
//
#define _WIN32_WINNT 0x0601
#include "app.h"
#include "asio.hpp"
#include "chrono"
#include "fileblock.h"
#include "format"
#include "iostream"
#include "spdlog/spdlog.h"

int main() {
    using namespace asio;
    using namespace mtft;
    App a;
    a.send(R"(c:\Users\xSeung\Desktop\MTD.TEST\A.mp4)", ip::address_v4::from_string("172.19.43.104"));
    a.send(R"(c:\Users\xSeung\Desktop\MTD.TEST\B.mp4)", ip::address_v4::from_string("172.19.43.104"));
    std::cin.get();
}