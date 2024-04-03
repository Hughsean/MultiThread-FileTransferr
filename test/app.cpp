﻿//
// Created by xSeung on 2023/4/20.
//
#define _WIN32_WINNT 0x0601
#include "app.h"
#include "iostream"
auto main() -> int {
    std::ios::sync_with_stdio(false);
    using namespace asio;
    using namespace mtft;
    try {
        App().run();
    }
    catch (std::exception& e) {
        std::cout << e.what();
        std::cin.get();
    }
}