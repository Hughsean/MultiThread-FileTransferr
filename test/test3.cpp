#define _WIN32_WINNT 0x0601
#include "asio.hpp"
#include "iostream"
int main() {
        asio::io_context        ioc;
        asio::ip::address_v4    addr(asio::ip::address_v4::from_string("192.168.43.75"));
        asio::ip::tcp::endpoint edp(addr, 8080);
        asio::ip::tcp::acceptor a(ioc, edp);
        std::cout << addr;
}