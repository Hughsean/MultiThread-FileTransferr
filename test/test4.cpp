#include "format"
#include "functional"
#include "future"
#include "iostream"
#include "mutex"
#include "pthread.h"
#include "queue"
#include "thread"
#include "vector"
class ThreadPool {
    public:
        explicit ThreadPool(size_t threadNum) : stop_(false) {
                for (size_t i = 0; i < threadNum; ++i) {
                        workers_.emplace_back([this]() {
                                for (;;) {
                                        std::function<void()> task;
                                        {
                                                std::unique_lock ul(mtx_);
                                                cv_.wait(ul, [this]() { return stop_ || !tasks_.empty(); });
                                                if (stop_ && tasks_.empty()) {
                                                        return;
                                                }
                                                task = std::move(tasks_.front());
                                                tasks_.pop();
                                        }
                                        task();
                                }
                        });
                }
        }

        ~ThreadPool() {
                {
                        std::unique_lock<std::mutex> ul(mtx_);
                        stop_ = true;
                }
                cv_.notify_all();
                for (auto &worker : workers_) {
                        worker.join();
                }
        }

        template <typename F, typename... Args>
        auto submit(F &&f, Args &&...args) -> std::future<decltype(f(args...))> {
                auto taskPtr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...));
                {
                        std::unique_lock<std::mutex> ul(mtx_);
                        if (stop_) {
                                throw std::runtime_error("submit on stopped ThreadPool");
                        }
                        tasks_.emplace([taskPtr]() { (*taskPtr)(); });
                }
                cv_.notify_one();
                return taskPtr->get_future();
        }

    private:
        bool                              stop_;
        std::vector<std::thread>          workers_;
        std::queue<std::function<void()>> tasks_;
        std::mutex                        mtx_;
        std::condition_variable           cv_;
};

// class Job {
//     private:
//         std::function<void(void *)> mfunc;
//         void                       *mdata;

//     public:
//         Job(/* args */ std::function<void(void *)> func, void *data);
//         ~Job(){};
// };
// Job::Job(std::function<void(void *)> func, void *data) : mfunc(func), mdata(data) {}
// template <typename T>

class TPool {
    private:
        /* data */
        std::vector<std::thread>          mworkers;
        std::queue<std::function<void()>> mjobs;
        std::mutex                        mtx;
        std::condition_variable           cv;
        const int                         n;
        bool                              mstop{ false };

    public:
        TPool(int n) : n(n) {
                // std::unique_lock<std::mutex>(mtx);
                for (size_t i = 0; i < n; i++) {
                        mworkers.push_back(std::thread([this, i]() {
                                while (true) {
                                        int                   id = i;
                                        std::function<void()> task;
                                        {
                                                std::unique_lock<std::mutex> lock(mtx);
                                                cv.wait(lock, [this] { return !mjobs.empty() || mstop; });
                                                if (mstop && mjobs.empty()) {
                                                        std::cout << std::format("{} over\n", id);
                                                        return;
                                                }
                                                task = std::move(mjobs.front());
                                                mjobs.pop();
                                        }
                                        task();
                                        /* code */
                                }
                        }));
                }
        };
        ~TPool() {
                {
                        std::unique_lock lock(mtx);
                        mstop = true;
                }
                cv.notify_all();
                for (auto &&e : mworkers) {
                        e.join();
                }
        };

        template <typename Func, typename... Args>
        auto submit(Func &&func, Args &&...args) -> std::future<decltype(func(args...))> {}
};

void *fun(void *) {
        std::cout << "pthread\n";
        return nullptr;
};
int main(int argc, char const *argv[]) {
        /* code */
        // std::mutex mx;
        // mx.lock();
        // mx.unlock();
        pthread_t id;
        pthread_create(&id, nullptr, fun, nullptr);
        pthread_join(id, nullptr);
        return 0;
}
