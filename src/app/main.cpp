// #include "cinatra.hpp"
// using namespace cinatra;

// int main() {
//     int max_thread_num = std::thread::hardware_concurrency();
//     coro_http_server server(max_thread_num, 8080);
//     server.set_http_handler<GET, POST>("/", [](coro_http_request& req, coro_http_response& res) {
//         res.set_status_and_content(status_type::ok, "hello world");
//     });

//     server.sync_start();
//     return 0;
// }
#include <iostream>
#include "spdlog/spdlog.h"

int main() {
    // 1. 直接使用默认logger记录日志，这将使用默认的sink（控制台彩色输出）
    spdlog::info("这是一条通过默认logger输出的信息");

    // 2. 获取默认logger并查看其sinks
    auto default_logger = spdlog::default_logger();
    auto& sinks = default_logger->sinks();


    std::cout << "默认logger包含 " << sinks.size() << " 个sink" << std::endl;

    // 3. 修改默认日志格式（会影响所有使用默认格式的sink）
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%L%$] [%s:%#] %v");  // 示例：添加文件名和行号:cite[1]:cite[5]

    // 4. 修改默认日志级别
    spdlog::set_level(spdlog::level::debug);  // 设置全局日志级别为debug:cite[1]:cite[3]

    spdlog::debug("这条debug信息现在可以显示了");

    return 0;
}