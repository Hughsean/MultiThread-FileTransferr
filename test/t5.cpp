#include "asio.hpp"
#include "iostream"
#include "json/json.h"

int main(int argc, char* args[]) {
        using namespace asio;
        streambuf   buf;
        Json::Value j;
        j["11"] = 11;
        std::cout << j.toStyledString();
}