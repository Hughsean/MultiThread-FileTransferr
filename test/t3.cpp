#define _WIN32_WINNT 0x0601
#include "app.h"
#include "asio.hpp"
#include "format"
#include "iostream"
#include "spdlog/spdlog.h"
#include "thread"
#include "vector"
#include "json/json.h"

int main(int argc, char const* argv[]) {
    using namespace mtft;
    std::thread t;
    if (t.joinable()) {
        t.join();
    }
}