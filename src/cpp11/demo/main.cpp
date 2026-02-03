// C++11 demo: shows basic C++11 features and usage of header-only libs
#include <iostream>
#include <thread>
#include <vector>
#include <asio.hpp>

int main() {
    std::cout << "cpp11 demo: threads + asio available via 3rd/include\n";
    std::vector<std::thread> ths;
    for (int i = 0; i < 2; ++i) ths.emplace_back([] { /* noop */ });
    for (auto &t : ths) t.join();
    return 0;
}
