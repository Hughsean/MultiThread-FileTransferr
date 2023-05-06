#include "asio.hpp"
#include "format"
#include "functional"
#include "future"
#include "iostream"
#include "mutex"
#include "queue"
#include "thread"
#include "vector"
#include "json/json.h"

int n            = 0;
using socket_ptr = std::shared_ptr<asio::ip::tcp::socket>;

void fun(int n) {
    using namespace asio;
    io_context        ioc;
    ip::tcp::endpoint edp(ip::address_v4::any(), n);
    ip::tcp::acceptor acp(ioc, edp);
    // acp.listen();
    auto sck = acp.accept();

    std::cout << sck.remote_endpoint().port() << std::endl;
}

int main(int argc, char const* argv[]) {
    using namespace asio;
    std::vector<std::thread> tvec;
    tvec.emplace_back(std::thread(fun, 8080));
    tvec.emplace_back(std::thread(fun, 8081));
    for (auto&& e : tvec) {
        e.join();
    }
}
