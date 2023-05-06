#define _WIN32_WINNT 0x0601
#include "asio.hpp"
#include "format"
#include "iostream"
#include "task.h"
#include "json/json.h"

int n;

using socket_ptr = std::shared_ptr<asio::ip::tcp::socket>;

void fun(int n) {
        using namespace asio;
        io_context        ioc;
        ip::tcp::endpoint edp(ip::address_v4::from_string("172.28.128.230"), n);
        ip::tcp::socket   sck(ioc);
        sck.connect(edp);
}

int main() {
        using namespace asio;
        std::vector<std::thread> tvec;
        tvec.emplace_back(std::thread(fun,8081));
        tvec.emplace_back(std::thread(fun,8080));
        for (auto &&e : tvec) {
                e.join();
        }
}