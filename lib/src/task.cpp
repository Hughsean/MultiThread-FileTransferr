//
// Created by xSeung on 2023/4/21.
//
#include "task.h"
#include "config.h"
#include "iostream"
namespace mtft {
    using namespace asio;

    Work::Work(LogAppender::ptr ptr) {
        mlog = ptr;
    }
    /// @brief buf ==> json
    /// @param buf 缓存
    /// @param json Json::Value
    /// @return 操作是否成功
    bool Work::ReadJson(streambuf& buf, Json::Value& json) {
        auto        size = buf.data().size();
        std::string str((const char*)buf.data().data(), size);
        json.clear();
        if (Json::Reader().parse(str, json)) {
            buf.consume(size);
            return true;
        }
        return false;
    }
    /// @brief json ==> buf
    /// @param buf 缓存
    /// @param json Json::Value
    void Work::WriteJson(streambuf& buf, Json::Value& json) {
        auto str  = json.toStyledString();
        auto size = str.size() * sizeof(char);
        json.clear();
        std::memcpy(buf.prepare(JSONSIZE).data(), str.data(), size);
        buf.commit(size);
    }
    UpWork::UpWork(LogAppender::ptr log, const ip::tcp::endpoint& edp, FileReader::ptr reader) : Work(log) {
        medp    = edp;
        mReader = reader;
        mid     = mReader->getID();
    }

    bool UpWork::uploadFunc(socket_ptr sck) {
        // FileReader::ptr freader;
        streambuf   buf;
        error_code  ec;
        uint32_t    size;
        uint32_t    progress;
        Json::Value json;
        // 接受JSON文件
        size = read(*sck.get(), buf.prepare(JSONSIZE), ec);
        if (ec) {
            mlog->log(LogLevel::INFO, LOG(std::format("work({}): {}", mid, ec.message())));
            return false;
        }
        // 解析JSON, 更改相应配置
        ReadJson(buf, json);
        progress = json[PROGRESS].asInt();
        mReader->seek(progress);
        // 发送文件
        while (!mReader->finished() && buf.data().size() != 0) {
            auto size = mReader->read((buf.prepare(BUFFSIZE).data()), BUFFSIZE);
            buf.commit(size);
            size = write(*sck, buf, ec);
            if (ec) {
                mlog->log(LogLevel::INFO, LOG(std::format("work({}): {}", mid, ec.message())));
                return false;
            }
            buf.consume(size);
        }
        // TODO:确认对方全部接收
        return true;
    }

    void UpWork::Func() {
        error_code ec;
        while (true) {
            auto sck = std::make_shared<ip::tcp::socket>(mioc);
            sck->connect(medp, ec);
            if (ec) {
                mlog->log(LogLevel::INFO, LOG(std::format("upwork({})创建连接: {}", mid, ec.message())));
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }
            else if (this->uploadFunc(sck)) {
                break;
            }
        }
        mlog->log(LogLevel::INFO, LOG(std::format("upwork({}): 已完成数据发送", mid)));
    }

    DownWork::DownWork(LogAppender::ptr log, FileWriter::ptr fwriter) : Work(log) {
        mFwriter = fwriter;
        mid      = fwriter->getID();
        medp     = ip::tcp::endpoint(ip::address_v4::any(), 0);
        macp     = std::make_shared<ip::tcp::acceptor>(mioc, medp);
    }

    bool DownWork::downloadFunc(socket_ptr sck) {
        streambuf   buf;
        error_code  ec;
        Json::Value json;
        uint32_t    size;
        // 向json写入配置
        json[PROGRESS] = mFwriter->getProgress();
        // json配置写入缓存, 并发送
        WriteJson(buf, json);
        size = write(*sck, buf.data(), ec);
        while (!mFwriter->finished()) {
            size = read(*sck, buf.prepare(BUFFSIZE), ec);
            buf.commit(size);
            size = mFwriter->write(buf.data().data(), size);
            buf.consume(size);
            if (ec) {
                mlog->log(LogLevel::INFO, LOG(std::format("downwork({}): {}", mid, ec.message())));
                return false;
            }
        }
        // TODO: 文件写完反馈
        return true;
    }

    void DownWork::Func() {
        error_code ec;
        while (true) {
            ip::tcp::acceptor acp(mioc, medp);
            acp.listen();
            auto sck = std::make_shared<ip::tcp::socket>(acp.accept(ec));
            if (ec) {
                mlog->log(LogLevel::INFO, LOG(std::format("downwork({}): {}", mid, ec.message())));
                continue;
            }
            else if (downloadFunc(sck)) {
                break;
            };
        }
        mFwriter->close();
        mlog->log(LogLevel::INFO, LOG(std::format("downwork({}): 已完成数据发送", mid)));
    }

    int DownWork::GetEdp() {
        return macp->local_endpoint().port();
    }

    Task::Task(LogAppender::ptr log, const std::vector<FileWriter::ptr>& vec) {
        for (auto&& e : vec) {
            mWorks.emplace_back(std::make_shared<DownWork>(log, e));
        }
    }

    Task::Task(LogAppender::ptr log, const std::vector<std::tuple<ip::tcp::endpoint, FileReader::ptr>>& vec) {
        for (auto&& [e, r] : vec) {
            mWorks.emplace_back(std::make_shared<UpWork>(log, e, r));
        }
    }

    void Task::run(int n) {
        mWorks.at(n)->Func();
    }
    int Task::getN() {
        return mWorks.size();
    }
}  // namespace mtft
