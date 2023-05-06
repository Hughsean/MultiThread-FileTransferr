//
// Created by xSeung on 2023/4/21.
//

#ifndef MAIN_TASK_H
#define MAIN_TASK_H
#define _WIN32_WINNT 0x0601

#include "asio.hpp"
#include "config.h"
#include "fileblock.h"
#include "queue"
#include "thread"
#include "json/json.h"

namespace mtft {
    using namespace asio;
    using socket_ptr = std::shared_ptr<ip::tcp::socket>;

    enum class WorkState { F, W, R };

    class Work {
    public:
        virtual void Func() = 0;
        Work(LogAppender::ptr log);
        virtual ~Work() = default;

    protected:
        bool              ReadJson(streambuf &buf, Json::Value &json);
        void              WriteJson(streambuf &buf, Json::Value &json);
        int               mid;
        io_context        mioc;
        LogAppender::ptr  mlog;
        ip::tcp::endpoint medp;
        WorkState         mState;
    };

    class UpWork : public Work {
    public:
        UpWork(LogAppender::ptr log, const ip::tcp::endpoint &edp, FileReader::ptr reader);
        void Func() override;

    private:
        bool uploadFunc(socket_ptr sck);
        FileReader::ptr mReader;
    };

    class DownWork : public Work {
    public:
        DownWork(LogAppender::ptr log, FileWriter::ptr fwriter);
        void Func() override;
        int  GetEdp();

    private:
        bool                               downloadFunc(socket_ptr sck);
        FileWriter::ptr                    mFwriter;
        std::shared_ptr<ip::tcp::acceptor> macp;
    };

    class Task {
    public:
        Task(LogAppender::ptr log, const std::vector<FileWriter::ptr> &vec);
        Task(LogAppender::ptr log, const std::vector<std::tuple<ip::tcp::endpoint, FileReader::ptr>> &vec);
        void run(int n);
        int  getN();

    private:
        std::vector<std::shared_ptr<Work>> mWorks;
    };

    /// @brief 作业池
    class TaskPool {
    public:
        TaskPool(int n);
        ~TaskPool();
        void submit(std::shared_ptr<Task> task);

    private:
        const int                         n;             // 线程个数
        std::queue<std::shared_ptr<Task>> mTaskQueue;    // 任务队列
        std::vector<std::thread>          mThreads;      // 工作线程
        std::shared_ptr<Task>             mCurrentTask;  // 当前任务
    };

}  // namespace mtft
#endif  // MAIN_TASK_H
