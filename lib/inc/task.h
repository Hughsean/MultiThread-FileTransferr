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

#include "asio/ip/tcp.hpp"
#include "asio/streambuf.hpp"
#include "condition_variable"
#include "config.h"
#include "fileblock.h"
#include "mutex"
#include "queue"
#include "thread"
#include "json/value.h"

namespace mtft {
    // using namespace asio;
    using socket_ptr = std::shared_ptr<asio::ip::tcp::socket>;

    enum class TaskType { Down, Up };
    /// @brief json.clear(); buf ==> json
    /// @param buf 缓存
    /// @param json Json::Value
    /// @return 操作是否成功
    void ReadJsonFromBuf(asio::streambuf &buf, Json::Value &json);
    /// @brief json ==> buf; json.clear()
    /// @param buf 缓存
    /// @param json Json::Value
    auto WriteJsonToBuf(asio::streambuf &buf, Json::Value &json) -> uint32_t;

    class Work : Base {
    public:
        using ptr                 = std::shared_ptr<Work>;
        virtual void Func(int id) = 0;
        explicit Work(int i);
        virtual ~Work();
        auto getID() const -> int;
        void stop();

    protected:
        int                     mid;
        asio::io_context        mioc;
        std::atomic_bool        mstop;
        std::condition_variable cstop;
    };

    class UpWork : public Work {
    public:
        UpWork(asio::ip::tcp::endpoint remote, const FileReader::ptr &reader);
        void Func(int id) override;

    private:
        auto                    uploadFunc(const socket_ptr &sck, int id) -> bool;
        FileReader::ptr         mReader;
        asio::ip::tcp::endpoint mremote;
    };

    class DownWork : public Work {
    public:
        explicit DownWork(const FileWriter::ptr &fwriter);
        void Func(int id) override;
        auto GetPort() -> int;

    private:
        FileWriter::ptr                          mFwriter;
        std::shared_ptr<asio::ip::tcp::acceptor> macp;
        asio::ip::tcp::endpoint                  medp;
        auto downloadFunc(const socket_ptr &sck, int id) -> bool;
    };

    class Task : public Base {
    public:
        using ptr = std::shared_ptr<Task>;
        Task()    = delete;
        Task(const std::vector<FileWriter::ptr> &vec, const std::string &fname);
        Task(const std::vector<std::tuple<asio::ip::tcp::endpoint, FileReader::ptr>> &vec,
             const std::string                                                       &fname);
        void               stop();
        auto               getWork() -> Work::ptr;
        auto               getPorts() -> std::vector<std::tuple<int, int>>;
        auto               getName() -> std::string;
        auto               getType() -> TaskType;
        [[nodiscard]] auto empty() const -> bool;

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
        void submit(const Task::ptr &task);
        auto isrepeat(const std::string &name) -> bool;

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
#endif
