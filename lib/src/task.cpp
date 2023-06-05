//
// Created by xSeung on 2023/4/21.
//
#include "task.h"
#include "filesystem"
#include "mutex"
#include "spdlog/spdlog.h"
#include "sstream"
#include "thread"

namespace mtft {
    using namespace asio;

    void ReadJsonFromBuf(streambuf& buf, Json::Value& json) {
        auto        size = buf.data().size();
        std::string str((const char*)buf.data().data(), size);
        json.clear();
        if (!Json::Reader().parse(str, json)) {
            throw std::exception("json解析失败");
        }
        buf.consume(size);
    }

    uint32_t WriteJsonToBuf(streambuf& buf, Json::Value& json) {
        auto str  = json.toStyledString();
        auto size = str.size() * sizeof(char);
        json.clear();
        std::memcpy(buf.prepare(size).data(), str.data(), size);
        buf.commit(size);
        return size;
    }

    Work::Work(int i) {
        mstop = false;
        mid   = i;
    }
    Work::~Work() {
        stop();
        cstop.notify_one();
    }
    int Work::getID() const {
        return mid;
    }
    void Work::stop() {
        mstop = true;
        cstop.notify_one();
    }

    UpWork::UpWork(const ip::tcp::endpoint& remote, const FileReader::ptr& reader) : Work(reader->getID()) {
        mremote = remote;
        mReader = reader;
    }

    bool UpWork::uploadFunc(const socket_ptr& sck, int id) {
        std::atomic_bool        exit{ false };
        std::thread             t;
        std::mutex              mtx;
        std::condition_variable cond;
        streambuf               buf;
        uint32_t                size;
        uint32_t                progress;
        Json::Value             json;
        bool                    fin = true;
        try {
            // 接收JSON文件
            size = sck->receive(buf.prepare(JSONSIZE));
            buf.commit(size);
            // 解析JSON, 更改相应配置
            ReadJsonFromBuf(buf, json);
            progress = json[PROGRESS].asInt();
            mReader->seek(progress);
            spdlog::info("thread(up )({:2}): 调整进度到{}", id, progress);
            // 发送文件
            t = std::thread([&] {
                while (!exit) {
                    std::unique_lock<std::mutex> _(mtx);
                    if (cond.wait_for(_, std::chrono::milliseconds(TIMEOUT)) == std::cv_status::timeout) {
                        spdlog::warn("thread(up )({:2}): 发送超时", id);
                        sck->cancel();
                        return;
                    }
                }
            });
            while ((!mReader->finished() || buf.data().size() != 0) && !mstop) {
                size = mReader->read(buf.prepare(BUFFSIZE).data(), BUFFSIZE);
                buf.commit(size);
                size = sck->send(buf.data());
                cond.notify_one();
                buf.consume(size);
            }
        }
        catch (const std::exception& e) {
            fin = false;
        }
        if (fin || mstop) {
            exit = true;
            cond.notify_one();
        }
        if (t.joinable()) {
            t.join();
        }
        return fin;
    }

    void UpWork::Func(int id) {
        error_code                       ec;
        bool                             fin{ false };
        std::shared_ptr<ip::tcp::socket> sck;
        auto                             t = std::thread([&, this] {
            std::mutex                   m;
            std::unique_lock<std::mutex> _(m);
            cstop.wait(_, [&, this] {
                if (mstop || fin) {
                    if (sck != nullptr) {
                        sck->cancel();
                    }
                    return true;
                }
                return false;
            });
        });
        while (!mstop) {
            sck = std::make_shared<ip::tcp::socket>(mioc);
            sck->connect(mremote, ec);
            if (ec) {
                std::this_thread::sleep_for(std::chrono::milliseconds(RECONNECTTIME));
            }
            else {
                spdlog::info("thread(up )({:2})连接到: {}", id, mremote.address().to_string());
                fin = uploadFunc(sck, id);
            }
            if (fin) {
                streambuf buf;
                sck->receive(buf.prepare(BUFFSIZE));
                break;
            }
        }
        cstop.notify_one();
        t.join();
        if (fin) {
            spdlog::info("thread(up )({:2}): 已完成数据发送", id);
        }
        mReader->close();
    }

    DownWork::DownWork(const FileWriter::ptr& fwriter) : Work(fwriter->getID()) {
        mFwriter = fwriter;
        medp     = ip::tcp::endpoint(ip::address_v4::any(), 0);
        macp     = std::make_shared<ip::tcp::acceptor>(mioc, medp);
        macp->listen();
    }

    bool DownWork::downloadFunc(const socket_ptr& sck, int id) {
        std::atomic_bool        exit{ false };
        std::thread             t;
        std::mutex              mtx;
        std::condition_variable cond;
        streambuf               buf;
        Json::Value             json;
        uint32_t                size;
        bool                    fin = true;
        try {
            // 向json写入配置
            json[PROGRESS] = mFwriter->getProgress();
            // json配置写入缓存, 并发送
            WriteJsonToBuf(buf, json);
            write(*sck, buf);
            t = std::thread([&] {
                while (!exit) {
                    std::unique_lock<std::mutex> _(mtx);
                    if (cond.wait_for(_, std::chrono::milliseconds(TIMEOUT)) == std::cv_status::timeout) {
                        spdlog::warn("thread(down)({:2}): 接收超时", id);
                        sck->cancel();
                        return;
                    }
                }
            });
            while (!mFwriter->finished() && !mstop) {
                size = sck->receive(buf.prepare(BUFFSIZE));
                cond.notify_one();
                buf.commit(size);
                size = mFwriter->write(buf.data().data(), size);
                buf.consume(size);
            }
        }
        catch (const std::exception& e) {
            fin = false;
        }
        if (fin || mstop) {
            exit = true;
            cond.notify_one();
        }
        if (t.joinable()) {
            t.join();
        }
        return fin;
    }

    void DownWork::Func(int id) {
        error_code ec;
        bool       fin{ false };
        auto       t = std::thread([&, this] {
            std::mutex                   m;
            std::unique_lock<std::mutex> _(m);
            cstop.wait(_, [&, this] {
                if (mstop || fin) {
                    macp->cancel();
                    return true;
                }
                return false;
            });
        });
        while (!mstop) {
            auto sck = std::make_shared<ip::tcp::socket>(macp->accept(ec));
            if (ec) {
                continue;
            }
            else {
                spdlog::info("thread(down)({:2})接收连接: {}", id, sck->remote_endpoint().address().to_string());
                fin = downloadFunc(sck, id);
            }
            if (fin) {
                sck->send(buffer(""));
                break;
            }
        }
        cstop.notify_one();
        t.join();
        if (fin) {
            spdlog::info("thread(down)({:2}): 已完成数据接收", id);
        }
        mFwriter->close();
    }

    int DownWork::GetPort() {
        return macp->local_endpoint().port();
    }

    Task::Task(const std::vector<FileWriter::ptr>& vec, const std::string& fname) {
        n     = (int)vec.size();
        fName = fname;
        type  = TaskType::Down;
        mWorks.reserve(vec.size());
        for (auto&& e : vec) {
            mWorks.emplace_back(std::make_shared<DownWork>(e));
        }
    }

    Task::Task(const std::vector<std::tuple<ip::tcp::endpoint, FileReader::ptr>>& vec, const std::string& fname) {
        n     = (int)vec.size();
        fName = fname;
        type  = TaskType::Up;
        mWorks.reserve(vec.size());
        for (auto&& [e, r] : vec) {
            mWorks.emplace_back(std::make_shared<UpWork>(e, r));
        }
    }

    void Task::stop() {
        for (auto&& e : mWorks) {
            e->stop();
        }
    }

    bool Task::empty() {
        return n == 0;
    }

    Work::ptr Task::getWork() {
        if (n == 0) {
            std::abort();
        }
        return mWorks.at(--n);
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
    //
    std::string Task::getName() {
        return fName;
    }

    TaskType Task::getType() {
        return type;
    }

    TaskPool::TaskPool() {
        mstop    = false;
        mCurrent = std::make_shared<Task>(std::vector<FileWriter::ptr>{}, "NULL");
        for (int i = 0; i < THREAD_N * ParallelN; i++) {
            // 工作线程
            mThreads.emplace_back([this, i] {
                spdlog::info("thread({:2}): 准备就绪", i);
                std::string name;
                TaskType    type;
                Work::ptr   work;
                bool        merge;
                while (true) {
                    merge = false;
                    {
                        // 访问mCurrent临界资源
                        std::unique_lock<std::mutex> _c(mtxC);
                        if (mCurrent->empty()) {
                            std::unique_lock<std::mutex> _q(mtxQ);
                            cond.wait(_q, [this] { return mstop || !mTaskQueue.empty(); });
                            if (mstop) {
                                break;
                            }
                            mCurrent = mTaskQueue.front();
                            mTaskQueue.pop();
                        }
                        work = mCurrent->getWork();
                        name = mCurrent->getName();
                        type = mCurrent->getType();
                    }
                    spdlog::info("thread({:2}): 开始工作", i);
                    work->Func(i);
                    if (mstop) {
                        break;
                    }
                    {
                        // 访问M临界变量
                        std::unique_lock<std::mutex> _(mtxM);
                        progressmap.at({ name, type })++;
                        if (progressmap.at({ name, type }) == THREAD_N) {
                            progressmap.erase({ name, type });
                            merge = type == TaskType::Down;
                        }
                    }
                    if (merge) {
                        spdlog::info("接收完成");
                        spdlog::info("开始合并: {}", name);
                        if (FileWriter::merge(name)) {
                            spdlog::info("合并完成");
                        }
                        std::filesystem::remove_all(fmt::format("{}{}", name, DIR));
                        spdlog::info("清理目录: {}{}", name, DIR);
                    }
                }
                spdlog::info("thread({:2})退出", i);
            });
        }
        spdlog::info("线程池初始化完成");
    }
    TaskPool::~TaskPool() {
        mstop = true;
        mCurrent->stop();
        cond.notify_all();
        for (auto&& e : mThreads) {
            e.join();
        }
    }

    bool TaskPool::isrepeat(const std::string& name) {
        bool ret = false;
        {
            std::unique_lock<std::mutex> _(mtxM);
            for (auto&& [tup, i] : progressmap) {
                auto&& [n, t] = tup;
                if (name == n && t == TaskType::Up) {
                    ret = true;
                    break;
                }
            }
        }
        cond.notify_one();
        return ret;
    }

    void TaskPool::submit(const Task::ptr& task) {
        // 访问mTaskQueue临界资源
        {
            std::unique_lock<std::mutex> _(mtxQ);
            std::unique_lock<std::mutex> _l(mtxM);
            progressmap.insert({ { task->getName(), task->getType() }, 0 });
            mTaskQueue.push(task);
        }
        cond.notify_one();
        spdlog::info("任务已提交");
    }
}  // namespace mtft
