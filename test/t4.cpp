#include "asio.hpp"
#include "format"
#include "functional"
#include "future"
#include "iostream"
#include "mutex"
#include "queue"
#include "thread"
#include "vector"
#include "json/json.h"

int n            = 0;
using socket_ptr = std::shared_ptr<asio::ip::tcp::socket>;

bool funa(socket_ptr ptr) {
        using namespace asio;
        error_code    ec;
        streambuf     buf;
        Json::Value   json;
        Json::OStream os(&buf);
        uint32_t      size;
        auto          jw = std::unique_ptr<Json::StreamWriter>(Json::StreamWriterBuilder().newStreamWriter());
        json["recvied"]  = n;
        std::cout << "json r:" << json["recvied"].asInt() << std::endl;
        jw->write(json, &os);
        size = ptr->write_some(buf.data(), ec);
        if (ec) {
                std::cerr << ec.message() << std::endl;
                return false;
        }
        buf.consume(size);
        while (true) {
                int ii[32];
                size = ptr->read_some(buffer(ii), ec);
                if (ec) {
                        std::cerr << ec.message() << std::endl;
                        return false;
                }
                n += size;
                std::cout << n << std::endl;
                if (n == 100) {
                        break;
                }
        }

        return true;
}

int main(int argc, char const* argv[]) {
        using namespace asio;
        io_context        ioc;
        ip::tcp::endpoint edp(ip::address_v4::from_string("172.28.128.168"), 8080);
        error_code        ec;
        auto              sck = ip::tcp::socket(ioc);
        streambuf         buf;
        sck.connect(edp);

        sck.read_some(buf.prepare(10), ec);
        std::cout << ec.message();

        // while (true) {
        //         auto ptr = std::make_shared<ip::tcp::socket>(ioc);
        //         ptr->connect(edp, ec);
        //         if (!ec) {
        //                 if (funa(ptr)) {
        //                         break;
        //                 }
        //         }
        //         else {
        //                 /* code */

        //                 std::cout << ec.message() << std::endl;
        //                 std::this_thread::sleep_for(std::chrono::seconds(2));
        //         }
        // }
}
