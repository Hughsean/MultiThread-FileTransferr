//
// Created by xSeung on 2023/4/20.
//
#define _WIN32_WINNT 0x0601
#include "asio.hpp"
#include "chrono"
#include "cstdint"
#include "filesystem"
#include "fmt/chrono.h"
#include "fmt/core.h"
#include "fstream"
#include "iostream"
#include "log.h"
#include "memory"
#include "string"
#include "thread"

using socket_ptr = std::shared_ptr<asio::ip::tcp::socket>;

void handler(socket_ptr ptr) {
        auto data = new std::byte[20];
        ptr.get()->receive(asio::buffer(data, 20));
}

int main() {
        using namespace asio;
        io_service        iso;
        ip::tcp::endpoint endpoint(ip::address_v4::from_string("192.168.43.75"),
                                   ip::port_type(8080));
        error_code        ec;
        ip::tcp::acceptor acceptor(iso, endpoint);
}