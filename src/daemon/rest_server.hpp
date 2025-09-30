#pragma once
#include <asio.hpp>
#include <functional>
#include <iostream>
#include <string>

#include "cinatra.hpp"

class RestServer {
  public:
    RestServer(asio::io_context& io_ctx, unsigned short port, std::string address)
        : server_(io_ctx, port, address) {}

    void SetupRoutes();

    cinatra::coro_http_server& server() { return server_; }

  private:
    cinatra::coro_http_server server_;
};