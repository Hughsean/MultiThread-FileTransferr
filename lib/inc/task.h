//
// Created by xSeung on 2023/4/21.
//

#ifndef MAIN_TASK_H
#define MAIN_TASK_H
#define _WIN32_WINNT 0x0601

#include "asio.hpp"
#include "config.h"
#include "fileblock.h"
#include "fstream"
#include "memory"
#include "queue"
#include "set"
#include "thread"

namespace cncd {
        using namespace asio;
        using socket_ptr = std::shared_ptr<ip::tcp::socket>;

        enum class WorkerState { F, W, R };

        class Work {
            public:
                virtual void func() = 0;
                virtual ~Work()     = default;
        };

        class UpWork : public Work {
            public:
                UpWork(socket_ptr sckptr, FileReader fbptr);
                void uploadFunc();
                // void setSocket(socket_ptr);
                void func() override;

            private:
                BYTE        mData[BLOCKSIZE];
                WorkerState mState;
                socket_ptr  mSckptr;
        };

        class DownWork : public Work {
            public:
                DownWork();
                void downloadFunc();
                void func() override;

            private:
        };

        class Task {
            public:
            private:
        };

        /// @brief 作业池
        class TaskPool {
            public:
                TaskPool();
                ~TaskPool();
                void submit(std::shared_ptr<Task> task);

            private:
                std::queue<std::shared_ptr<Task>> mTaskQueue;    // 任务队列
                std::vector<std::thread>          mThreads;      // 工作线程
                std::shared_ptr<Task>             mCurrentTask;  // 当前任务
        };

}  // namespace cncd
#endif  // MAIN_TASK_H
