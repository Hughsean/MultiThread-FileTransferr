#define _WIN32_WINNT 0x0601
#include "app.h"
#include "asio.hpp"
#include "fileblock.h"
#include "fstream"
#include "iostream"
#include "spdlog/spdlog.h"
#include "string"
#include "json/json.h"

int main() {
    using namespace asio;
    using namespace mtft;
    namespace log = spdlog;
    App a;
    a.tcplisten();
    std::cin.get();
}
// }
// void fun() {
//     using namespace asio;
//     std::string                       ip1 = "192.168.43.75";
//     std::string                       ip2 = "172.28.16.1";
//     io_context                        ioc;
//     ip::udp::endpoint                 uendp(ip::udp::v4(), 8080);
//     ip::udp::socket                   usck(ioc, uendp);
//     std::unique_ptr<Json::CharReader> jreader(Json::CharReaderBuilder().newCharReader());
//     streambuf                         buf;
//     Json::Value                       root;
//     std::istream                      ist(&buf);
//     Json::String                      err;
//     try {
//         /* code */
//         ip::udp::endpoint fromendp(ip::address_v4::any(), 8080);
//         uint32_t          size = usck.receive_from(buf.prepare(1024), fromendp);
//         buf.commit(size);
//         char* p = (char*)buf.data().begin()->data();
//         jreader.get()->parse(p, p + size, &root, &err);
//         std::cout << root << std::endl;
//         std::cout << err + "\n";
//     }
//     catch (const system_error& e) {
//         std::cerr << e.what() << '\n';
//     }
// }
// /// @brief TCP 接收文件测试
// void fun1() {
//     using namespace asio;
//     std::string       ip = "172.19.43.33";
//     io_context        ioc;
//     ip::tcp::endpoint edp(ip::address_v4::from_string(ip), 8080);
//     ip::tcp::socket   sck(ioc);
//     streambuf         buf;
//     std::ofstream     ofs(R"(C:\Users\xSeung\Desktop\OUT.mp4)", std::ios::binary);
//     int               size = 1024;
//     error_code        ec;
//     sck.connect(edp, ec);
//     if (ec) {
//         std::cout << ec.message() << std::endl;
//         return;
//     }
//     int i = 0;
//     while (true) {
//         try {
//             sck.receive(buf.prepare(size));
//             buf.commit(size);
//             ofs.write((char*)buf.data().data(), (LONGLONG)buf.data().size());
//             i++;
//             if (i % 1000 == 0) {
//                 std::cout << "接受1024000字节" << buf.data().size() << std::endl;
//             }
//             buf.consume(size);
//         }
//         catch (const system_error& e) {
//             std::cerr << e.code().message() << '\n';
//             break;
//         }
//     }
//     sck.shutdown(ip::tcp::socket::shutdown_both);
//     ofs.close();
// }
