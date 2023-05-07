//
// Created by xSeung on 2023/4/21.
//
#include "app.h"
#include "config.h"
namespace mtft {
    void App::send(std::string fPath) {}

    void App::receive() {
        io_context        ioc;
        error_code        ec;
        ip::tcp::endpoint edp(ip::address_v4::any(), TCPPORT);
        ip::tcp::acceptor acp(ioc, edp);
        while (!mstop) {
            /* code */

        }
    }

    void App::listen() {
        io_context        ioc;
        ip::udp::endpoint edp(ip::address_v4::any(), UDPPORT);
        ip::udp::socket   sck(ioc, edp);
        streambuf         buf;
        error_code        ec;
        Json::Value       json;
        ip::udp::endpoint remote;

        while (!mstop) {
            auto size = sck.receive_from(buf.prepare(JSONSIZE), remote);
            buf.commit(size);
            ReadJsonFromBuf(buf, json);
            if (json[BROADCAST].asString() == BROADCAST) {}
            else if (json[RESPONSE].asString() == RESPONSE) {
                // addEdp(ip::tcp::));
            };
        }
    }

}  // namespace mtft
