#define _WIN32_WINNT 0x0601
#include "asio.hpp"
#include "format"
#include "iostream"
#include "task.h"
#include "json/json.h"

int n;

using socket_ptr = std::shared_ptr<asio::ip::tcp::socket>;

bool funa(socket_ptr ptr) {
        using namespace asio;
        Json::Value   json;
        error_code    ec;
        streambuf     buf;
        Json::Reader  jr;
        Json::IStream jis(&buf);
        uint32_t      size;

        size = ptr->read_some(buf.prepare(1024), ec);
        if (ec) {
                std::cerr << ec.message() << std::endl;
                return false;
        }
        buf.commit(size);
        // std::cout << "jis.rdbuf: \n" << jis.rdbuf() << std::endl;
        if (!jr.parse(jis, json)) {
                std::cerr << "解析失败" << std::endl;
        }
        buf.consume(size);
        n = json["recvied"].asInt();
        std::cout << "客户端n:" << n << std::endl;
        while (true) {
                size = ptr->write_some(buffer("1"), ec);
                if (ec) {
                        std::cerr << ec.message() << std::endl;
                        return false;
                }
                n += size;
                std::cout << n << std::endl;
                if (n == 100) {
                        break;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        return true;
}

int main() {
        using namespace asio;
        // std::cout << "DEBUG" << std::endl;
        io_context        ioc;
        ip::tcp::endpoint edp(ip::tcp::v4(), 8080);
        ip::tcp::acceptor acp(ioc, edp);
        error_code        ec;
        auto              p = acp.accept(ec);
        // p.shutdown(socket_base::shutdown_send);
        p.close();
        // p.close();
        std::this_thread::sleep_for(std::chrono::seconds(10));
        // while (true) {
        //         auto p = std::make_shared<ip::tcp::socket>(acp.accept(ec));
        //         if (!ec) {
        //                 std::cout << p->remote_endpoint().port() << std::endl;
        //                 if (funa(p)) {
        //                         break;
        //                 }
        //         }
        //         else {
        //                 std::cout << ec.message() << std::endl;
        //                 std::this_thread::sleep_for(std::chrono::seconds(2));
        //         }
        // }
}