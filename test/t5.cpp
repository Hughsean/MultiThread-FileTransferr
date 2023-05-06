#include "asio.hpp"
#include "connection.h"
#include "ctime"
#include "iostream"
#include "json/json.h"
/////////////////

/////////////////
int main(int argc, char* args[]) {
    using namespace asio;
    using namespace mtft;
    auto     l = std::make_shared<LogAppender>(std::cout);
    auto     f = std::make_shared<FileWriter>(1, "1", "", 1, 1, l);
    DownWork d(l, f);
    std::cout << d.GetEdp();
}