#pragma once
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/signal_set.hpp>
#include <asio/write.hpp>
#include <cstdio>

using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
using asio::ip::tcp;
namespace this_coro = asio::this_coro;

namespace lynx {
class EchoServer {
  public:
    EchoServer(asio::io_context& io_ctx, unsigned short port, std::string address);

    awaitable<void> listener();
    awaitable<void> echo(tcp::socket socket);

  private:
    unsigned short port_;
    std::string address_;
};
}  // namespace lynx