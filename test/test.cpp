#include "fileblock.h"
#include "fstream"
#include "thread"
int main() {
    using namespace mtft;
    std::ifstream ifs(R"(c:\Users\xSeung\Desktop\MTD.TEST\A.mp4)");
    auto          size = ifs.seekg(0, std::ios::end).tellg();
    ifs.close();
    auto                     vecr = FileReader::Build(THREAD_N, size, R"(c:\Users\xSeung\Desktop\MTD.TEST\A.mp4)");
    auto                     vecw = FileWriter::Build(THREAD_N, size, "A.mp4", "A.mp4.tmp");
    std::vector<std::thread> vt;
    vt.reserve(3);
    for (int i = 0; i < THREAD_N; i++) {
        vt.emplace_back([&, i] {
            auto r = vecr.at(i);
            auto w = vecw.at(i);
            char buf[1024];
            while (!r->finished()) {
                r->read(buf, 1024 * sizeof(char));
                w->write(buf, 1024 * sizeof(char));
            }
            w->close();
        });
    }
    for (auto&& e : vt) {
        e.join();
    }
    FileWriter::merge("A.mp4");
}