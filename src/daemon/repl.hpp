#pragma once

#include <cli/cli.h>
#include <cli/clifilesession.h>
#include <cli/clilocalsession.h>
#include <cli/standaloneasioremotecli.h>
#include <cli/standaloneasioscheduler.h>

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

// a custom struct to be used as a user-defined parameter type
struct Bar {
    string to_string() const { return std::to_string(value); }
    friend istream& operator>>(istream& in, Bar& p);
    int value;
};

class UserREPL {
  public:
    explicit UserREPL(IoContext& iocontext);
    ~UserREPL() = default;

    // 禁止拷贝构造和赋值操作
    UserREPL(const UserREPL&) = delete;
    UserREPL& operator=(const UserREPL&) = delete;

    // 允许移动构造和赋值操作
    UserREPL(UserREPL&&) = default;
    UserREPL& operator=(UserREPL&&) = default;

    void StartLocalSession();
    void StartTelnetSession(int port = 5000);
    void StartFileSession(std::istream& in = std::cin, std::ostream& out = std::cout);
    void Stop() { scheduler.Stop(); }

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