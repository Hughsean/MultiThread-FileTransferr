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
    using namespace asio;
    io_context              ioc;
    auto                    acp = std::make_shared<ip::tcp::acceptor>(ioc, ip::tcp::endpoint(ip::tcp::v4(), 8080));
    std::mutex              m;
    std::condition_variable cv;
    bool                    b    = false;
    bool                    stop = true;
    acp->cancel();
    std::thread t([&] {
        { std::unique_lock<std::mutex> _(m); }
        try {
            // b = true;
            spdlog::info("format_string_t<Args...> fmt");
            acp->accept();
            b = true;
        }
        catch (std::exception& e) {
            spdlog::error(e.what());
            return;
        }
        cv.notify_one();
    });
    {
        std::unique_lock<std::mutex> _(m);
        auto                         p = cv.wait_for(_, std::chrono::milliseconds(2000), [&] { return !b; });
        spdlog::info(p);
    }
    if (true) {
        spdlog::info("const T &msg");
        cv.notify_one();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        acp->cancel();
    }
    t.join();
}
