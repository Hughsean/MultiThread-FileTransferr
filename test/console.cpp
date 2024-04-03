#define _WIN32_WINNT 0x0601
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"
#include "config.h"
#include "iostream"
#include "regex"

auto main() -> int {
    using namespace asio;
    io_context        ioc;
    ip::tcp::socket   sck(ioc, ip::tcp::endpoint(ip::tcp::v4(), 0));
    std::string       cmd;
    const std::string _exit{ R"(\s*exit\s*)" };
    try {
        sck.connect(ip::tcp::endpoint(ip::address_v4::loopback(), mtft::CORESPONSEPORT));
        while (true) {
            std::cout << ">>";
            std::getline(std::cin, cmd);
            sck.send(buffer(cmd));
            if (std::regex_match(cmd, std::regex(_exit))) {
                break;
            }
        }
    }
    catch (const std::exception& e) {
        std::cout << e.what() << '\n';
    }
    return 0;
}