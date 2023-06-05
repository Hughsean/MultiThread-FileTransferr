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

    class Work : Base {
    public:
        using ptr                 = std::shared_ptr<Work>;
        virtual void Func(int id) = 0;
        explicit Work(int i);
        virtual ~Work();
        int  getID() const;
        void stop();

    protected:
        int                     mid;
        io_context              mioc;
        std::atomic_bool        mstop;
        std::condition_variable cstop;
    };

    class UpWork : public Work {
    public:
        UpWork(const ip::tcp::endpoint &remote, const FileReader::ptr &reader);
        void Func(int id) override;

    private:
        bool              uploadFunc(const socket_ptr &sck, int id);
        FileReader::ptr   mReader;
        ip::tcp::endpoint mremote;
    };

    class DownWork : public Work {
    public:
        explicit DownWork(const FileWriter::ptr &fwriter);
        void Func(int id) override;
        int  GetPort();

    private:
        bool                               downloadFunc(const socket_ptr &sck, int id);
        FileWriter::ptr                    mFwriter;
        std::shared_ptr<ip::tcp::acceptor> macp;
        ip::tcp::endpoint                  medp;
    };

    class Task : public Base {
    public:
        using ptr = std::shared_ptr<Task>;
        Task()    = delete;
        Task(const std::vector<FileWriter::ptr> &vec, const std::string &fname);
        Task(const std::vector<std::tuple<ip::tcp::endpoint, FileReader::ptr>> &vec, const std::string &fname);
        void                              stop();
        bool                              empty();
        Work::ptr                         getWork();
        std::vector<std::tuple<int, int>> getPorts();
        std::string                       getName();
        TaskType                          getType();

    private:
        std::vector<Work::ptr> mWorks;
        TaskType               type;
        std::string            fName;
        int                    n;
    };

    /**
     * @brief 作业池
     *
     * @version 0.1
     * @author 鳯玖 (xSeung@163.com)
     * @date 2023-05-10
     * @copyright Copyright (c) 2023
     */
    class TaskPool : Base {
    public:
        TaskPool();
        ~TaskPool();
        bool isrepeat(const std::string &name);
        void submit(const Task::ptr &task);

    private:
        volatile bool                                    mstop;       //
        std::queue<Task::ptr>                            mTaskQueue;  // 任务队列
        std::vector<std::thread>                         mThreads;    // 工作线程
        Task::ptr                                        mCurrent;    // 当前任务
        std::condition_variable                          cond;        // 工作条件
        std::mutex                                       mtxC;        // mCurrent锁
        std::mutex                                       mtxQ;        // mTaskQueue锁
        std::mutex                                       mtxM;        // map锁
        std::map<std::tuple<std::string, TaskType>, int> progressmap;
    };
}  // namespace mtft
#endif  // MAIN_TASK_H
