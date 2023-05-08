//
// Created by xSeung on 2023/4/21.
//
#include "task.h"
#include "config.h"
#include "mutex"
#include "spdlog/spdlog.h"

namespace mtft {
    using namespace asio;
    // namespace log = spdlog;

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

    uint32_t WriteJsonToBuf(streambuf& buf, Json::Value& json) {
        auto str  = json.toStyledString();
        auto size = str.size() * sizeof(char);
        json.clear();
        std::memcpy(buf.prepare(size).data(), str.data(), size);
        buf.commit(size);
        return size;
    }
    // XXX:test
    // void Work::Func() {
    //     log::info("{}", mid);
    //     std::this_thread::sleep_for(std::chrono::seconds(10));
    // }

    // Work::Work(int i) {
    //     mid = i;
    // }
    ///////////
    Work::Work(int i) {
        mstop = false;
        mid   = i;
    }
    int Work::getID() {
        return mid;
    }
    void Work::stop() {
        mstop = true;
    }

    UpWork::UpWork(const ip::tcp::endpoint& remote, const FileReader::ptr& reader) : Work(reader->getID()) {
        mremote = remote;
        mReader = reader;
    }

    bool UpWork::uploadFunc(const socket_ptr& sck) {
        // FileReader::ptr freader;
        streambuf   buf;
        error_code  ec;
        uint32_t    size;
        uint32_t    progress;
        Json::Value json;
        // 接收JSON文件
        size = read(*sck.get(), buf.prepare(JSONSIZE), ec);
        if (ec) {
            spdlog::warn("work({}): {}", mid, ec.message());
            return false;
        }
        // 解析JSON, 更改相应配置
        ReadJsonFromBuf(buf, json);
        progress = json[PROGRESS].asInt();
        mReader->seek(progress);
        // 发送文件
        while (!mReader->finished() && buf.data().size() != 0 && !mstop) {
            size = mReader->read((buf.prepare(BUFFSIZE).data()), BUFFSIZE);
            buf.commit(size);
            size = write(*sck, buf, ec);
            if (ec) {
                spdlog::warn("work({}): {}", mid, ec.message());
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
                spdlog::warn("upwork({})创建连接: {}", mid, ec.message());
                std::this_thread::sleep_for(std::chrono::seconds(5));
                continue;
            }
            else if (this->uploadFunc(sck)) {
                break;
            }
        }
        spdlog::info("upwork({}): 已完成数据发送", mid);
    }

    DownWork::DownWork(const FileWriter::ptr& fwriter) : Work(fwriter->getID()) {
        mFwriter = fwriter;
        medp     = ip::tcp::endpoint(ip::address_v4::any(), 0);
        macp     = std::make_shared<ip::tcp::acceptor>(mioc, medp);
        macp->listen();
    }

    bool DownWork::downloadFunc(const socket_ptr& sck) {
        streambuf   buf;
        error_code  ec;
        Json::Value json;
        uint32_t    size;
        // 向json写入配置
        json[PROGRESS] = mFwriter->getProgress();
        // json配置写入缓存, 并发送
        WriteJsonToBuf(buf, json);
        size = write(*sck, buf.data(), ec);
        if (ec) {
            spdlog::warn("downwork({}): {}", mid, ec.message());
            return false;
        }
        buf.consume(size);
        while (!mFwriter->finished() && !mstop) {
            size = read(*sck, buf.prepare(BUFFSIZE), ec);
            buf.commit(size);
            size = mFwriter->write(buf.data().data(), size);
            buf.consume(size);
            if (ec) {
                spdlog::warn("downwork({}): {}", mid, ec.message());
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
                spdlog::warn("downwork({}): {}", mid, ec.message());
                continue;
            }
            else if (downloadFunc(sck)) {
                break;
            };
        }
        mFwriter->close();
        spdlog::info("downwork({}): 已完成数据发送", mid);
    }

    int DownWork::GetPort() {
        return macp->local_endpoint().port();
    }

    Task::Task(const std::vector<FileWriter::ptr>& vec) {
        type = TaskType::Down;
        mWorks.reserve(vec.size());
        for (auto&& e : vec) {
            mWorks.emplace_back(std::make_shared<DownWork>(e));
        }
    }

    Task::Task(const std::vector<std::tuple<ip::tcp::endpoint, FileReader::ptr>>& vec) {
        type = TaskType::Up;
        mWorks.reserve(vec.size());
        for (auto&& [e, r] : vec) {
            mWorks.emplace_back(std::make_shared<UpWork>(e, r));
        }
    }
    // Task::Task(const std::vector<Work::ptr>& vec) {
    //     for (auto&& e : vec) {
    //         mWorks.emplace_back(e);
    //     }
    // }
    void Task::stop() {
        for (auto&& e : mWorks) {
            e->stop();
        }
    }
    uint32_t Task::getN() {
        return mWorks.size();
    }
    Work::ptr Task::getWork(int i) {
        return mWorks.at(i);
    }
    /**
     * @brief 返回下载任务各进程监听的对应端口
     *
     * @return std::vector<std::tuple<int, int>> id, port
     */
    std::vector<std::tuple<int, int>> Task::getPorts() {
        if (type != TaskType::Down) {
            spdlog::error("{}:{}", __FILE__, __LINE__);
            abort();
        }
        std::vector<std::tuple<int, int>> vec;
        vec.reserve(mWorks.size());
        for (auto&& e : mWorks) {
            vec.emplace_back(e->getID(), dynamic_cast<DownWork*>(e.get())->GetPort());
        }
        return vec;
    }

    TaskPool::TaskPool(int n) : num(n) {
        mstop    = false;
        mCurrent = nullptr;
        finish   = new std::atomic<bool>[n];
        for (int i = 0; i < n; i++) {
            finish[i] = false;
            // 工作线程
            mThreads.emplace_back([this, i] {
                while (true) {
                    Work::ptr work;
                    // 访问mCurrent临界资源
                    {
                        std::unique_lock<std::mutex> _(mtxC);
                        condC.wait(_, [this, i] { return mstop || (mCurrent != nullptr && !finish[i]); });
                        if (mstop) {
                            break;
                        }
                        spdlog::info("工作线程thread({}): 开始工作", i);
                        work = mCurrent->getWork(i);
                    }
                    work->Func();
                    finish[i] = true;
                    condC.notify_all();
                }
                spdlog::info("工作线程thread({})退出", i);
            });
        }
        // 调度线程
        mThreads.emplace_back([this, n] {
            while (!mstop) {
                // 访问mCurrent临界资源
                {
                    {
                        std::unique_lock<std::mutex> _(mtxC);
                        condC.wait(_, [this] {
                            return mstop || ((allFinish() || mCurrent == nullptr) && !mTaskQueue.empty());
                        });
                        if (mstop) {
                            break;
                        }
                        // 访问mTaskQueue临界资源
                        {
                            std::unique_lock<std::mutex> _ul(mtxQ);
                            mCurrent = mTaskQueue.front();
                            mTaskQueue.pop();
                        }
                        Reset();
                    }
                    condC.notify_all();
                    spdlog::info("调度线程thread({})完成一次调度", n);
                }
            }
            spdlog::info("调度线程thread({})退出", n);
        });
        spdlog::info("线程池初始化完成");
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
    void TaskPool::submit(const Task::ptr& task) {
        // 访问mTaskQueue临界资源
        {
            std::unique_lock<std::mutex> _(mtxQ);
            mTaskQueue.push(task);
        }
        condC.notify_all();
        condQ.notify_one();
        spdlog::info("任务已提交");
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
