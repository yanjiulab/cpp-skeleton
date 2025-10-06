#pragma once

#include "cli/cli.h"
#include "cli/clifilesession.h"
#include "cli/clilocalsession.h"
#include "cli/standaloneasioremotecli.h"
#include "cli/standaloneasioscheduler.h"
#include "tabulate/table.hpp"

using namespace tabulate;
using Row_t = Table::Row_t;

using namespace cli;
using namespace std;

#if ASIO_VERSION < 101200
using IoContext = asio::io_service;
#else
using IoContext = asio::io_context;
#endif

namespace cli {
// using MainScheduler = StandaloneAsioScheduler;
using CliTelnetServer = StandaloneAsioCliTelnetServer;
}  // namespace cli

namespace lynx {
// a custom struct to be used as a user-defined parameter type
struct Bar {
    string to_string() const { return std::to_string(value); }
    friend istream& operator>>(istream& in, Bar& p);
    int value;
};

class Repl {
  public:
    explicit Repl(IoContext& iocontext);
    ~Repl() = default;

    // 禁止拷贝构造和赋值操作
    Repl(const Repl&) = delete;
    Repl& operator=(const Repl&) = delete;

    // 允许移动构造和赋值操作
    Repl(Repl&&) = default;
    Repl& operator=(Repl&&) = default;

    void start_local_terminal_session();
    void start_telnet_session(int port = 5000);
    void start_file_session(std::istream& in = std::cin, std::ostream& out = std::cout);
    void stop() { scheduler.Stop(); }

  private:
    StandaloneAsioScheduler scheduler;
    unique_ptr<Cli> cli;
    CmdHandler colorCmd;
    CmdHandler nocolorCmd;

  public:
    unique_ptr<CliLocalTerminalSession> local_session;
    unique_ptr<CliTelnetServer> telnet_session;
    unique_ptr<CliFileSession> file_session;
};
}  // namespace lynx