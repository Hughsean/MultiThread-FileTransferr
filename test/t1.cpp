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
// #include "spdlog/spdlog.h"
// #include "spdlog/sinks/basic_file_sink.h"
int main() {
    using namespace asio;
    using namespace mtft;
    // auto file_logger = spdlog::basic_logger_mt("file_logger", "logs.txt");
    // 设置全局默认日志记录器为文件日志记录器
    // spdlog::set_default_logger(file_logger);
    App().run();
}