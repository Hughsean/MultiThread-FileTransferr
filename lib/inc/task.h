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

namespace cncd {
        using namespace asio;
        using socket_ptr = std::shared_ptr<ip::tcp::socket>;
        using frmap_ptr  = std::shared_ptr<std::map<int, FileReader::ptr>>;
        using fwmap_ptr  = std::shared_ptr<std::map<int, FileWriter::ptr>>;

        enum class WorkState { F, W, R };

        class Work {
            public:
                virtual void Func() = 0;
                Work(LogAppender::ptr log, const ip::tcp::endpoint &edp);
                virtual ~Work() = default;
                bool ReadJson(streambuf &buf, Json::Value &json);
                void WriteJson(streambuf &buf, Json::Value &json);

            protected:
                // private:
                int               mid;
                io_context        mioc;
                streambuf         mBuf;
                WorkState         mState;
                socket_ptr        mSckptr;
                LogAppender::ptr  mlog;
                ip::tcp::endpoint medp;
        };

        class UpWork : public Work {
            public:
                UpWork(LogAppender::ptr log, const ip::tcp::endpoint &edp);
                void Func() override;

            private:
                bool      uploadFunc(socket_ptr sck);
                frmap_ptr mReaders;
        };

        class DownWork : public Work {
            public:
                DownWork(LogAppender::ptr log, const ip::tcp::endpoint &edp);
                void Func() override;

            private:
                bool            downloadFunc();
                FileWriter::ptr mFwriter;
        };

        class Task {
            public:
                Task();

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

}  // namespace cncd
#endif  // MAIN_TASK_H
