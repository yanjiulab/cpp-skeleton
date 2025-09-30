#include "asio.hpp"
#include "cinatra.hpp"

using namespace cinatra;

int main() {
    asio::io_context iocontext;

    coro_http_server server(iocontext, 8080, "0.0.0.0");

    server.set_http_handler<GET, POST>(
        R"(/numbers/(\d+)/test/(\d+))", [](request& req, response& res) {
            std::cout << " matches[1] is : " << req.matches_[1]
                      << " matches[2] is: " << req.matches_[2] << std::endl;

            res.set_status_and_content(status_type::ok, "hello world");
        });

    server.async_start();

    asio::signal_set exit_signals(iocontext, SIGINT, SIGTERM);
    exit_signals.async_wait([&](std::error_code ec, int signo) {
        // std::cout << "ioctx is stop: " << iocontext.stopped() << std::endl;
        std::thread([&]{server.stop();}).detach();
        // server.stop();
        // iocontext.stop();
        // std::cout << "ioctx is stop: " << iocontext.stopped() << std::endl;
    });

    iocontext.run();

    std::cout << "Goodbye and thanks for all the fish." << std::endl;
}