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
    using namespace mtft;
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
    // write(SyncWriteStream &s, buf);
}