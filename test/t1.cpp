//
// Created by xSeung on 2023/4/20.
//
#define _WIN32_WINNT 0x0601
#include "asio.hpp"
#include "chrono"
#include "cstdint"
#include "fileblock.h"
#include "filesystem"
#include "fmt/chrono.h"
#include "fmt/core.h"
#include "fstream"
#include "iostream"
#include "log.h"
#include "memory"
#include "string"
#include "thread"
#include "json/json.h"

/// @brief UDP广播测试server
void fun2() {
        using namespace asio;
        using namespace cncd;
        Json::Value       root;
        io_service        ioc;
        ip::udp::endpoint uendp(ip::udp::v4(), 0);
        error_code        ec;
        ip::udp::socket   usck(ioc, uendp);
        char              cs[14]{ "你好世界!" };
        usck.set_option(socket_base::broadcast(true));
        root["host"] = "1";
        root["int"]  = 12;

        // Json::StreamWriterBuilder           swb;
        std::unique_ptr<Json::StreamWriter> p(Json::StreamWriterBuilder().newStreamWriter());
        asio::streambuf                     buf;
        // Json::OStream;
        Json::OStream os(&buf);
        p.get()->write(root, &os);
        // p.get()->write(root, buf);
        try {
                ip::udp::endpoint toendp(ip::address_v4::broadcast(), 8080);
                std::cout << usck.send_to(buf.data(), toendp);
        }
        catch (const system_error &e) {
                std::cerr << e.what() << '\n';
        }
}
/// @brief fileblock 测试函数
void fun3() {
        using namespace asio;
        using namespace cncd;
        std::string      readpath(R"(C:\Users\xSeung\Videos\Captures\A.mp4)");
        int              pos   = readpath.find_last_of('\\');
        int              n     = 3;
        std::string      fname = readpath.substr(pos + 1);
        std::string      writepath(R"(C:\Users\xSeung\Desktop)");
        uint32_t         totalsize = std::ifstream(readpath).seekg(0, std::ios::end).tellg();
        uint32_t         blocksize = totalsize / (n - 1);
        uint32_t         lastblock = totalsize - (n - 1) * blocksize;
        LogAppender::ptr log       = std::make_shared<LogAppender>(std::cout);
        std::cout << std::format("{} {} {} {}\n", fname, totalsize, blocksize, lastblock);
        auto vecr = FileReader::ReadersBuilder(n, readpath, log);
        auto vecw = FileWriter::fwsCreator(n, totalsize, fname, writepath, log);
        // for (auto &&e : vecr) {
        //         auto ee = e.get();
        //         std::cout << std::format("len {} off {}\n", ee->mlength, ee->moffect);
        // }
        // for (auto &&e : vecw) {
        //         auto ee = e.get();
        //         std::cout << std::format("len {} off {}\n", ee->mlength, ee->moffset);
        // }

        auto func = [&](int i) {
                auto r = vecr.at(i).get();
                auto w = vecw.at(i).get();
                while (!r->finished()) {
                        /* code */
                        BYTE     buf[1024];
                        uint32_t size = r->read(buf, 1024);
                        if (size != w->write(buf, size)) {
                                abort();
                        };
                }
                w->close();
                std::cout << std::format("{} over\n", i);
        };
        std::vector<std::thread> vt;
        for (int i = 0; i < n; i++) {
                vt.emplace_back(std::thread(func, i));
        }
        for (auto &&e : vt) {
                e.join();
        }
        std::cout << "开始合并\n";
        std::ofstream ofs(writepath + R"(\)" + fname, std::ios::binary);
        for (auto &&e : vecw) {
                auto        ee = e.get();
                auto        fn = ee->getFname();
                std::string fp = writepath + R"(\)" + fn;
                std::cout << fp << std::endl;
                std::ifstream infs(fp, std::ios::binary);
                if (infs.good()) {
                        ofs << infs.rdbuf();
                }
                else {
                        return;
                }
        }
        ofs.close();
}
/// @brief TCP传输文件测试
void fun4() {
        using namespace asio;
        io_context        ioc;
        ip::tcp::endpoint edp(ip::tcp::v4(), 8080);
        ip::tcp::acceptor acp(ioc, edp);
        streambuf         buf;
        auto              sck = acp.accept();
        std::cout << sck.remote_endpoint().address() << std::endl;
        std::ifstream infs(R"(C:\Users\xSeung\Desktop\MTD.TEST\B.mp4)", std::ios::binary);
        int           size = 1024;
        int           i    = 0;
        while (!infs.eof()) {
                infs.read((char *)buf.prepare(size).data(), size);
                buf.commit(size);
                sck.send(buf.data());
                buf.consume(size);
                i++;
                if (i % 1000 == 0) {
                        std::cout << "发送1024000字节" << std::endl;
                }
        }
        sck.shutdown(ip::tcp::socket::shutdown_both);
}
int main() {
        using namespace asio;
        io_context        ioc;
        ip::tcp::endpoint edp(ip::tcp::v4(), 8080);
        ip::tcp::acceptor acp(ioc, edp);
        streambuf         buf;
        auto              sck = acp.accept();
        std::cout << std::format("{}:{}", sck.remote_endpoint().address().to_string(), sck.remote_endpoint().port());
        sck.wait(socket_base::wait_type::wait_error);
}