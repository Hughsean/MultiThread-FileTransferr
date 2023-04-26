#define _WIN32_WINNT 0x0601
#include "asio.hpp"
#include "iostream"
#include "string"
#include "json/json.h"
void fun(asio::ip::tcp::socket& sck) {
        sck.close();
        std::cout << "closed\n";
}
int main() {
        using namespace asio;
        std::string ip1 = "192.168.43.75";
        std::string ip2 = "172.21.42.9";
        // char                                data[100];
        io_context                          ioc;
        ip::udp::endpoint                   uendp(ip::udp::v4(), 8080);
        ip::udp::socket                     usck(ioc, uendp);
        asio::streambuf                     buf;
        // streambuf::mutable_buffers_type     mbuf;
        std::unique_ptr<Json::StreamWriter> p(Json::StreamWriterBuilder().newStreamWriter());
        Json::OStream                       os(&buf);
        Json::Value                         root;
        try {
                /* code */
                ip::udp::endpoint fromendp(ip::address_v4::any(), 8080);
                // uint32_t          size = usck.receive_from(mbuf, fromendp);
                // buf.commit(size);
                // std::cout << size << std::endl;
                p.get()->write(root, &os);
                std::cout << root << std::endl;
        }
        catch (const system_error& e) {
                std::cerr << e.what() << '\n';
        }
}
