#pragma once

#include <complex>

#include "cli/cli.h"
#include "cli/clifilesession.h"
#include "cli/clilocalsession.h"
#include "cli/standaloneasioremotecli.h"
#include "cli/standaloneasioscheduler.h"
#include "cli/filehistorystorage.h"

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

// a free function to be used as handler
static void foo(std::ostream& out, int x) { out << x << std::endl; }

// a custom struct to be used as a user-defined parameter type
struct Bar {
    string to_string() const { return std::to_string(value); }
    friend istream& operator>>(istream& in, Bar& p);
    int value;
};

// needed only for generic help, you can omit this
namespace cli {
template <>
struct TypeDesc<Bar> {
    static const char* Name() { return "<bar>"; }
};
}  // namespace cli

// needed only for generic help, you can omit this
namespace cli {
template <>
struct TypeDesc<complex<double>> {
    static const char* Name() { return "<complex>"; }
};
}  // namespace cli

class Repl {
  public:
    explicit Repl(IoContext& iocontext);
    ~Repl() = default;

    Repl(const Repl&) = delete;
    Repl& operator=(const Repl&) = delete;

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
