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
    io_context ioc;
    error_code ec;
    auto       p  = std::make_shared<ip::tcp::acceptor>(ioc, ip::tcp::endpoint(ip::tcp::v4(), 8080));
    auto       pp = std::async(std::launch::async, [&]() {
        try {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        catch (std::exception& e) {
            spdlog::warn("{}", e.what());
        }
    });

    switch (pp.wait_for(std::chrono::seconds(2))) {
    case std::future_status::timeout:
        spdlog::info("超时");
        p->cancel();
        break;
    default:
        break;
    }
}