//
// Created by xSeung on 2023/4/21.
//
#include "task.h"
#include "config.h"
#include "spdlog/spdlog.h"
namespace mtft {
    using namespace asio;
    namespace log = spdlog;

    bool ReadJsonFromBuf(streambuf& buf, Json::Value& json) {
        auto        size = buf.data().size();
        std::string str((const char*)buf.data().data(), size);
        json.clear();
        if (Json::Reader().parse(str, json)) {
            buf.consume(size);
            return true;
        }
        return false;
    }

    void WriteJsonToBuf(streambuf& buf, Json::Value& json) {
        auto str  = json.toStyledString();
        auto size = str.size() * sizeof(char);
        json.clear();
        std::memcpy(buf.prepare(JSONSIZE).data(), str.data(), size);
        buf.commit(size);
    }
    // XXX:test
    void Work::Func() {
        log::info("{}", mid);
        // std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Work::Work(int i) {
        mid = i;
    }

    Work::Work() {
        mstop = false;
    }

    void Work::stop() {
        mstop = true;
    }

    UpWork::UpWork(const ip::tcp::endpoint& remote, FileReader::ptr reader) {
        mremote = remote;
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
        // 接收JSON文件
        size = read(*sck.get(), buf.prepare(JSONSIZE), ec);
        if (ec) {
            log::warn("work({}): {}", mid, ec.message());
            return false;
        }
        // 解析JSON, 更改相应配置
        ReadJsonFromBuf(buf, json);
        progress = json[PROGRESS].asInt();
        mReader->seek(progress);
        // 发送文件
        while (!mReader->finished() && buf.data().size() != 0 && !mstop) {
            auto size = mReader->read((buf.prepare(BUFFSIZE).data()), BUFFSIZE);
            buf.commit(size);
            size = write(*sck, buf, ec);
            if (ec) {
                log::warn("work({}): {}", mid, ec.message());
                return false;
            }
            buf.consume(size);
        }
        // TODO:确认对方全部接收
        return true;
    }

    void UpWork::Func() {
        error_code ec;
        while (!mstop) {
            auto sck = std::make_shared<ip::tcp::socket>(mioc);
            sck->connect(mremote, ec);
            if (ec) {
                log::warn("upwork({})创建连接: {}", mid, ec.message());
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }
            else if (this->uploadFunc(sck)) {
                break;
            }
        }
        log::info("upwork({}): 已完成数据发送", mid);
    }

    DownWork::DownWork(FileWriter::ptr fwriter) {
        mFwriter = fwriter;
        mid      = fwriter->getID();
        medp     = ip::tcp::endpoint(ip::address_v4::any(), 0);
        macp     = std::make_shared<ip::tcp::acceptor>(mioc, medp);
        macp->listen();
    }

    bool DownWork::downloadFunc(socket_ptr sck) {
        streambuf   buf;
        error_code  ec;
        Json::Value json;
        uint32_t    size;
        // 向json写入配置
        json[PROGRESS] = mFwriter->getProgress();
        // json配置写入缓存, 并发送
        WriteJsonToBuf(buf, json);
        size = write(*sck, buf.data(), ec);
        while (!mFwriter->finished() && !mstop) {
            size = read(*sck, buf.prepare(BUFFSIZE), ec);
            buf.commit(size);
            size = mFwriter->write(buf.data().data(), size);
            buf.consume(size);
            if (ec) {
                log::warn("downwork({}): {}", mid, ec.message());
                return false;
            }
        }
        // TODO: 文件写完反馈
        return true;
    }

    void DownWork::Func() {
        error_code ec;
        while (!mstop) {
            ip::tcp::acceptor acp(mioc, medp);
            acp.listen();
            auto sck = std::make_shared<ip::tcp::socket>(acp.accept(ec));
            if (ec) {
                log::warn("downwork({}): {}", mid, ec.message());
                continue;
            }
            else if (downloadFunc(sck)) {
                break;
            };
        }
        mFwriter->close();
        log::info("downwork({}): 已完成数据发送", mid);
    }

    int DownWork::GetPort() {
        return macp->local_endpoint().port();
    }

    Task::Task(const std::vector<FileWriter::ptr>& vec) {
        for (auto&& e : vec) {
            mWorks.emplace_back(std::make_shared<DownWork>(e));
        }
    }

    Task::Task(const std::vector<std::tuple<ip::tcp::endpoint, FileReader::ptr>>& vec) {
        for (auto&& [e, r] : vec) {
            mWorks.emplace_back(std::make_shared<UpWork>(e, r));
        }
    }
    Task::Task(const std::vector<Work::ptr>& vec) {
        for (auto&& e : vec) {
            mWorks.emplace_back(e);
        }
    }
    void Task::stop() {
        for (auto&& e : mWorks) {
            e->stop();
        }
    }
    int Task::getN() {
        return mWorks.size();
    }
    Work::ptr Task::getWork(int i) {
        return mWorks.at(i);
    }

    TaskPool::TaskPool(int n) : num(n) {
        mstop    = false;
        mCurrent = nullptr;
        finish   = new std::atomic<bool>[n];
        for (size_t i = 0; i < n; i++) {
            // FIX
            finish[i] = false;
            // 工作线程
            mThreads.emplace_back([this, i] {
                while (true) {
                    Work::ptr work;
                    // 访问mCurrent临界资源
                    {
                        std::unique_lock _(mtxC);
                        condC.wait(_, [this, i] { return mstop || (mCurrent != nullptr && !finish[i]); });
                        log::info("{}: 进入临界区", i);
                        if (mstop) {
                            break;
                        }
                        work = mCurrent->getWork(i);
                    }
                    work->Func();
                    finish[i] = true;
                    condC.notify_one();
                }
                log::info("thread({})退出", i);
            });
        }
        // 调度线程
        mThreads.emplace_back([this, n] {
            while (!mstop) {
                // 访问mCurrent临界资源
                {
                    {
                        std::unique_lock _(mtxC);
                        condC.wait(_, [this] {
                            return mstop || ((allFinish() || mCurrent == nullptr) && !mTaskQueue.empty());
                        });
                        if (mstop) {
                            break;
                        }
                        // 访问mTaskQueue临界资源
                        {
                            std::unique_lock __(mtxQ);
                            mCurrent = mTaskQueue.front();
                            mTaskQueue.pop();
                        }
                        Reset();
                    }
                    condC.notify_all();
                }
            }
            log::info("thread({})退出", n);
        });
        log::info("线程池初始化完成");
    }
    TaskPool::~TaskPool() {
        mstop = true;
        if (mCurrent != nullptr) {
            mCurrent->stop();
        }
        condC.notify_all();
        delete finish;
        for (auto&& e : mThreads) {
            e.join();
        }
    }
    void TaskPool::submit(Task::ptr task) {
        {
            std::unique_lock _(mtxQ);
            mTaskQueue.push(task);
        }
        condC.notify_all();
        condQ.notify_one();
        log::info("任务已提交");
    }
    bool TaskPool::allFinish() {
        bool f{ true };
        for (size_t i = 0; i < num; i++) {
            f = f && finish[i];
        }
        return f;
    }
    void TaskPool::Reset() {
        for (size_t i = 0; i < num; i++) {
            finish[i] = false;
        }
    }
}  // namespace mtft
