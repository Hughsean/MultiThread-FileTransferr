#define _WIN32_WINNT 0x0601
#include "app.h"
#include "asio.hpp"
#include "iostream"
#include "regex"
#include "spdlog/spdlog.h"
#include "json/json.h"

/////////////////
/////////////////
void fun() {
    asio::io_context        ioc;
    asio::ip::tcp::acceptor acp(ioc, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), 8080));
    asio::streambuf         buf;
    auto                    sck = acp.accept();
    asio::read(sck, buf);
    spdlog::info(buf.data().size());
}
int main(int argc, char const* argv[]) {
    using namespace mtft;
    std::thread     t(fun);
    io_context      ioc;
    ip::tcp::socket sck(ioc);
    Json::Value     json;
    streambuf       buf;
    sck.connect(ip::tcp::endpoint(ip::address_v4::from_string("172.28.128.225"), 8080));
    json["1"] = 100;
    WriteJsonToBuf(buf, json);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    spdlog::info("main:{}", buf.data().size());
    write(sck, buf);
    spdlog::info("main:{}", buf.data().size());
    t.join();
    // write(SyncWriteStream &s, buf);
}
