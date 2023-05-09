//
// Created by xSeung on 2023/4/21.
//
/**
 * @brief 任务模块
 *
 * @version 0.1
 * @author 鳯玖 (xSeung@163.com)
 * @date 2023-04-21
 * @copyright Copyright (c) 2023
 */

#ifndef MAIN_TASK_H
#define MAIN_TASK_H
#define _WIN32_WINNT 0x0601

#include "asio.hpp"
#include "condition_variable"
#include "config.h"
#include "fileblock.h"
#include "mutex"
#include "queue"
#include "thread"
#include "json/json.h"

namespace mtft {
    using namespace asio;
    using socket_ptr = std::shared_ptr<ip::tcp::socket>;

    enum class TaskType { Down, Up };
    /// @brief json.clear(); buf ==> json
    /// @param buf 缓存
    /// @param json Json::Value
    /// @return 操作是否成功
    void ReadJsonFromBuf(streambuf &buf, Json::Value &json);
    /// @brief json ==> buf; json.clear()
    /// @param buf 缓存
    /// @param json Json::Value
    uint32_t WriteJsonToBuf(streambuf &buf, Json::Value &json);

    class Work {
    public:
        using ptr = std::shared_ptr<Work>;
        // XXX:test
        virtual void Func() = 0;
        // explicit Work(int i);
        //
        explicit Work(int i);
        Work(Work &)  = delete;
        Work(Work &&) = delete;
        int  getID();
        void stop();
        virtual ~Work() = default;

    protected:
        int        mid;
        io_context mioc;
        bool       mstop;
    };

    class UpWork : public Work {
    public:
        UpWork(const ip::tcp::endpoint &remote, const FileReader::ptr &reader);
        UpWork(UpWork &&) = delete;
        UpWork(UpWork &)  = delete;
        void Func() override;

    private:
        bool              uploadFunc(const socket_ptr &sck);
        FileReader::ptr   mReader;
        ip::tcp::endpoint mremote;
    };

    class DownWork : public Work {
    public:
        DownWork(const FileWriter::ptr &fwriter);
        DownWork(DownWork &&) = delete;
        DownWork(DownWork &)  = delete;
        void            Func() override;
        int             GetPort();
        FileWriter::ptr getFw();

    private:
        bool                               downloadFunc(const socket_ptr &sck);
        FileWriter::ptr                    mFwriter;
        std::shared_ptr<ip::tcp::acceptor> macp;
        ip::tcp::endpoint                  medp;
    };

    class Task {
    public:
        using ptr = std::shared_ptr<Task>;
        Task();
        Task(const std::vector<FileWriter::ptr> &vec, const std::string &fname);
        Task(const std::vector<std::tuple<ip::tcp::endpoint, FileReader::ptr>> &vec, const std::string &fname);
        Task(Task &)  = delete;
        Task(Task &&) = delete;
        void                              stop();
        bool                              empty();
        Work::ptr                         getWork();
        std::vector<std::tuple<int, int>> getPorts();
        std::vector<std::string>      getVec();
        std::string                       getName();
        TaskType                          getType();

    private:
        std::vector<Work::ptr> mWorks;
        TaskType               type;
        std::string            fName;
    };

    /// @brief 作业池
    class TaskPool {
    public:
        explicit TaskPool();
        ~TaskPool();
        void submit(const Task::ptr &task);
        TaskPool(TaskPool &)            = delete;
        TaskPool(TaskPool &&)           = delete;
        TaskPool operator=(TaskPool &)  = delete;
        TaskPool operator=(TaskPool &&) = delete;

    private:
        volatile bool            mstop;       //
        std::queue<Task::ptr>    mTaskQueue;  // 任务队列
        std::vector<std::thread> mThreads;    // 工作线程
        Task::ptr                mCurrent;    // 当前任务
        std::condition_variable  condw;       //
        std::condition_variable  conds;       //
        std::mutex               mtxC;        // mCurrent锁
        std::mutex               mtxQ;        // mTaskQueue锁
        std::atomic_int          finish;
    };

}  // namespace mtft
#endif  // MAIN_TASK_H
