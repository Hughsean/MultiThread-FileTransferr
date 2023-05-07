#define _WIN32_WINNT 0x0601
#include "app.h"
#include "asio.hpp"
#include "iostream"
#include "spdlog/spdlog.h"
#include "json/json.h"
/////////////////
/////////////////
class Pool {
public:
    Pool(int n);

private:
};
int main(int argc, char const* argv[]) {
    using namespace mtft;
    // Work().Func();
    // TaskPool               tp(4);
    // std::vector<Work::ptr> vec{ std::make_shared<Work>(0), std::make_shared<Work>(1), std::make_shared<Work>(2),
    //                             std::make_shared<Work>(3) };
    // auto                   t = std::make_shared<Task>(vec);
    // tp.submit(t);
    // std::this_thread::sleep_for(std::chrono::seconds(2));
    // tp.submit(t);
    // std::this_thread::sleep_for(std::chrono::seconds(3));
    // tp.submit(t);
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
