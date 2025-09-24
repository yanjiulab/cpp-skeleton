#include "main.h"

#include <iostream>

int main(int argc, char* argv[]) {
    // printf("hello %s\n", std::string("world").c_str());

    // spdlog_demo();
    // vector_demo();
    // json_demo();
    // asio_20_echo_demo();
    asio_20_chat_demo(argc, argv);

    return 0;
}