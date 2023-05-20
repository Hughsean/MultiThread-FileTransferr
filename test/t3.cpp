#include "iostream"
#include "regex"
#include "string"

int main() {
    const std::string send{ R"(send\s+(\b(?:\d{1,3}\.){3}\d{1,3}\b)\s+\"?([^"]*[^\\^"])\"?\s*)" };
    const std::string scan{ "(scan)" };
    const std::string help{ "(help)" };
    std::smatch       res;
    std::string       cmd;
    std::getline(std::cin, cmd);
    if (std::regex_match(cmd, res, std::regex(send))) {
        for (const auto& e : res) {
            std::cout << e << std::endl;
        }
    }
}
