#include "asio.hpp"
int main() {
    using namespace asio;
    io_context        ioc;
    ip::tcp::acceptor a(ioc, ip::tcp::endpoint(ip::tcp::v4(), 8080));
    std::thread       t([&] {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    });
    a.accept();
    t.join();
}