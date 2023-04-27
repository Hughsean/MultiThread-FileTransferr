#include "functional"
#include "future"
#include "iostream"
#include "mutex"
#include "thread"

int fun() {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return 1;
}

int main(int argc, char* args[]) {
        auto e = std::async(std::launch::async, fun);
        std::cout << "r: ";
        std::cout << e.get();
}