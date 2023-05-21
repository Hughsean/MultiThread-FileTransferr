#include <iostream>

int main(int argc, char* argv[]) {
    // 输出命令行参数
    for (int i = 0; i < argc; ++i) {
        std::cout << "Argument " << i << ": " << argv[i] << std::endl;
    }

    return 0;
}
