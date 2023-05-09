#define _WIN32_WINNT 0x0601
#include "app.h"
#include "asio.hpp"
#include "fileblock.h"
#include "fstream"
#include "iostream"
#include "spdlog/spdlog.h"
#include "string"
#include "json/json.h"

int main() {
    using namespace asio;
    using namespace mtft;
    namespace log = spdlog;
    App a;
    a.tcplisten();
    std::cin.get();
}