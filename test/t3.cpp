#include "iostream"
#include "regex"

int main() {
    std::string s{ R"(^(.*[\\/])?([^\\/]+)$)" };
    std::regex  reg(s);
    std::getline(std::cin, s);
    if (std::regex_match(s, reg)) {
        std::cout << "m\n";
    };
}