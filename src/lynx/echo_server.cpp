//
// echo_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "echo_server.hpp"

namespace lynx {
EchoServer::EchoServer(asio::io_context& io_ctx, unsigned short port, std::string address)
    : port_(port), address_(address) {
    co_spawn(io_ctx, listener(), detached);
}

awaitable<void> EchoServer::echo(tcp::socket socket) {
    try {
        char data[1024];
        for (;;) {
            std::size_t n = co_await socket.async_read_some(asio::buffer(data), use_awaitable);
            co_await async_write(socket, asio::buffer(data, n), use_awaitable);
        }
    } catch (std::exception& e) {
        std::printf("echo Exception: %s\n", e.what());
    }
}

awaitable<void> EchoServer::listener() {
    auto executor = co_await this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), port_});
    for (;;) {
        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
        co_spawn(executor, echo(std::move(socket)), detached);
    }
}
}  // namespace lynx

// int main() {
//     try {
//         asio::io_context io_context(1);

//         asio::signal_set signals(io_context, SIGINT, SIGTERM);
//         signals.async_wait([&](auto, auto) { io_context.stop(); });

//         co_spawn(io_context, listener(), detached);

//         io_context.run();
//     } catch (std::exception& e) {
//         std::printf("Exception: %s\n", e.what());
//     }
// }