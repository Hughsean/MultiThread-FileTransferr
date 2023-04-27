#include "format"
#include "functional"
#include "future"
#include "iostream"
#include "mutex"
#include "queue"
#include "thread"
#include "vector"


const int n = 3;

class T {
    private:
        int                     flag{ n };
        std::condition_variable cv;
        std::mutex              mtx;
        std::queue<std::thread> tq;

    public:
        T(/* args */) {
                auto func = [this](int i) {
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                        {
                                std::unique_lock _(mtx);
                                std::cout << std::format("{} get mutex\n", i);
                                flag -= 1;
                        }
                        std::cout << std::format("{} over\n", i);
                        cv.notify_one();
                };
                // int i=0;
                for (int i = 0; i < n; i++) {
                        tq.emplace(std::thread(func, i));
                }
        };
        ~T() {
                std::unique_lock _(mtx);
                std::cout << std::format("~ get mutex\n");
                cv.wait(_, [this] {
                        std::cout << std::format("flag: {}\n", flag);
                        return flag == 0;
                });
        }
};

int main(int argc, char const* argv[]) {
        T();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return 0;
}
