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
int main() {
    using namespace asio;
    using namespace mtft;
    App().run();
}