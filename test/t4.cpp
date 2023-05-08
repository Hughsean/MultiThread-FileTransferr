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
    asio::io_context        ioc;
    asio::ip::tcp::acceptor acp(ioc, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), 8080));
    asio::streambuf         buf;
    auto                    sck = acp.accept();
    // asio::read(sck, buf);
    auto size = sck.receive(buf.prepare(100));
    buf.commit(size);
    spdlog::info(buf.data().size());
    // write(SyncWriteStream &s, buf);
}
