#define _WIN32_WINNT 0x0601
#include "asio.hpp"
#include "iostream"
#include "string"

void fun(asio::ip::tcp::socket& sck) {
        sck.close();
        std::cout << "closed\n";
}
int main() {
        using namespace asio;
        std::string       ip1 = "192.168.43.75";
        std::string       ip2 = "127.0.0.1";
        std::byte         data[100];
        io_context        ios;
        ip::tcp::socket   sck(ios);
        ip::tcp::endpoint endpoint(ip::address::from_string(ip1), ip::port_type(8080));
        error_code        err;
}
