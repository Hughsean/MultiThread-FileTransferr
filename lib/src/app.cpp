//
// Created by xSeung on 2023/4/21.
//
#include "app.h"
#include "config.h"
#include "filesystem"
#include "spdlog/spdlog.h"
#include "task.h"
#include "json/value.h"

namespace mtft {

    App::App() : mpool(THREAD_N) {
        mstop = false;
        // mpath = DIR;
        // mudplisten = std::thread([this] { udplisten(); });
        // mtcplisten = std::thread([this] { tcplisten(); });
        edpvec.reserve(10);
        std::filesystem::create_directories(DIR);
        spdlog::info("工作目录: {}", DIR);
    }
    App::~App() {
        mstop = true;
        // io_context ioc;
        // ip::udp::socket usck(ioc, ip::udp::endpoint(ip::udp::v4(), 0));
        // ip::tcp::socket tsck(ioc, ip::tcp::endpoint(ip::tcp::v4(), 0));
        // usck.send_to(buffer(""), ip::udp::endpoint(ip::address_v4::loopback(), UDPPORT));
        // tsck.connect(ip::tcp::endpoint(ip::address_v4::loopback(), TCPPORT));
        // usck.close();
        // mudplisten.join();
        // mtcplisten.join();
        std::filesystem::remove_all(DIR);
    }

    void App::send(const std::string& fPath, const ip::address_v4& ip) {
        std::ifstream infs(fPath, std::ios::binary);
        if (!infs.good()) {
            spdlog::warn("文件({})读取失败", fPath);
            return;
        }
        io_context        ioc;
        ip::tcp::endpoint edp(ip, TCPPORT);
        ip::tcp::socket   sck(ioc);
        Json::Value       json;
        streambuf         buf;
        try {
            sck.connect(edp);
            // 获取文件信息[name, size]
            int64_t totalsize = infs.seekg(0, std::ios::end).tellg();
            auto    rvec      = FileReader::Builder(THREAD_N, totalsize, fPath);
            auto    name      = fPath.substr(fPath.find_last_of('\\') + 1);
            spdlog::info("name: {} size:{}", name, totalsize);
            // 信息写入json, 并发送到对端
            json[FILENAME] = name;
            json[FILESIZE] = totalsize;
            WriteJsonToBuf(buf, json);
            auto size = write(sck, buf);
            buf.consume(size);
            // 接收对端传回的配置信息
            size = sck.receive(buf.prepare(JSONSIZE));
            buf.commit(size);
            ReadJsonFromBuf(buf, json);
            std::vector<std::tuple<ip::tcp::endpoint, FileReader::ptr>> vec;
            vec.reserve(json.size());
            for (auto iter = json.begin(); iter != json.end(); iter++) {
                auto id   = (*iter)[ID].asInt();
                auto port = (*iter)[PORT].asInt();
                vec.emplace_back(ip::tcp::endpoint(ip, port), rvec.at(id));
            }
            json = Json::Value();
            //
            auto task = std::make_shared<Task>(vec, name);
            json[OK]  = OK;
            WriteJsonToBuf(buf, json);
            write(sck, buf);
            mpool.submit(task);
        }
        catch (const std::exception& e) {
            spdlog::warn("send {} to {}: {}", fPath, ip.to_string(), e.what());
        }
    }

    void App::receive() {}

    void App::scan() {
        io_context        ioc;
        streambuf         buf;
        Json::Value       json;
        ip::udp::endpoint remote(ip::address_v4::broadcast(), UDPPORT);
        ip::udp::socket   sck(ioc, ip::udp::endpoint(ip::udp::v4(), 0));
        sck.set_option(socket_base::broadcast(true));
        json[TYPE] = SCAN;
        WriteJsonToBuf(buf, json);
        sck.send_to(buf.data(), remote);
    }

    void App::udplisten() {
        io_context        ioc;
        streambuf         buf;
        error_code        ec;
        Json::Value       json;
        ip::udp::endpoint remote;
        ip::udp::endpoint edp(ip::address_v4::any(), UDPPORT);
        ip::udp::socket   sck(ioc, edp);
        while (true) {
            auto size = sck.receive_from(buf.prepare(JSONSIZE), remote);
            if (remote.address().is_loopback()) {
                break;
            }
            buf.commit(size);
            ReadJsonFromBuf(buf, json);
            auto str = json[TYPE].asString();
            if (str == SCAN) {
                // 响应扫描请求
                json.clear();
                json[TYPE] = RESPONSE;
                WriteJsonToBuf(buf, json);
                buf.consume(sck.send_to(buf.data(), ip::udp::endpoint(remote.address(), UDPPORT)));
            }
            else if (str == RESPONSE) {
                edpvec.emplace_back(remote.address().to_v4());
                spdlog::info("扫描到{}", remote.address().to_string());
            }
        }
    }
    void App::tcplisten() {
        io_context        ioc;
        streambuf         buf;
        Json::Value       json;
        Json::Value       elem;
        ip::tcp::acceptor acp(ioc, ip::tcp::endpoint(ip::address_v4::any(), TCPPORT));
        while (true) {
            try {
                auto sck = acp.accept();
                // 回环地址意为关机信号
                if (sck.remote_endpoint().address().is_loopback()) {
                    break;
                }
                // 接收文件信息, 配置相应的数据结构
                // auto size = read(sck, buf.prepare(JSONSIZE));
                auto size = sck.receive(buf.prepare(JSONSIZE));
                buf.commit(size);
                ReadJsonFromBuf(buf, json);
                auto totalsize = json[FILESIZE].asInt();
                auto name      = json[FILENAME].asString();
                spdlog::info("name:{} size:{}", name, totalsize);
                auto wvec = FileWriter::Builder(THREAD_N, totalsize, name, DIR);
                auto task = std::make_shared<Task>(wvec, name);
                // 将FileWriter id对应的端口发送给对方
                auto id_port = task->getPorts();
                json         = Json::Value();
                for (auto&& [id, port] : id_port) {
                    elem[ID]   = id;
                    elem[PORT] = port;
                    json.append(elem);
                }
                WriteJsonToBuf(buf, json);
                write(sck, buf);
                sck.receive(buf.prepare(JSONSIZE));
                mpool.submit(task);
            }
            catch (const error_code& ec) {
                spdlog::warn(ec.message());
            }
        }
    }
    void App::interpreter(const std::string& cmd) {
        if (cmd == "scan") {
            scan();
        }
        // else if () {}
    }
}  // namespace mtft
