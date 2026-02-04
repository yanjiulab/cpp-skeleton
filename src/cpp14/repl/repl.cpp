#include "repl.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

using namespace cli;
using namespace std;

struct GlobalStatus {
    int timeout;
} g_status;

istream& operator>>(istream& in, Bar& p) {
    in >> p.value;
    return in;
}

Repl::Repl(IoContext& iocontext) : scheduler(iocontext) {
    auto rootMenu = make_unique<Menu>("cli");
    rootMenu->Insert(
        "free_function",
        foo,
        "Call a free function that echoes the parameter passed");
    rootMenu->Insert(
        "hello",
        [](std::ostream& out) { out << "Hello, world\n"; },
        "Print hello world");
    rootMenu->Insert(
        "hello_everysession",
        [](std::ostream&) { Cli::cout() << "Hello, everybody" << std::endl; },
        "Print hello everybody on all open sessions");
    rootMenu->Insert(
        "timeout",
        [](std::ostream& out, int sec) {
            g_status.timeout = sec;
            out << "Set timeout to: " << g_status.timeout << std::endl;
        },
        "Set timeout");
    rootMenu->Insert(
        "timeout",
        [](std::ostream& out) { out << "timeout: " << g_status.timeout << std::endl; },
        "Get timeout");
    rootMenu->Insert(
        "answer",
        [](std::ostream& out, int x) { out << "The answer is: " << x << "\n"; },
        "Print the answer to Life, the Universe and Everything");
    rootMenu->Insert(
        "file",
        [](std::ostream& out, int fd) {
            out << "file descriptor: " << fd << "\n";
        },
        "Print the file descriptor specified",
        {"file_descriptor"});
    rootMenu->Insert(
        "echo", {"string to echo"},
        [](std::ostream& out, const string& arg) {
            out << arg << "\n";
        },
        "Print the string passed as parameter");
    rootMenu->Insert(
        "echo", {"first string to echo", "second string to echo"},
        [](std::ostream& out, const string& arg1, const string& arg2) {
            out << arg1 << ' ' << arg2 << "\n";
        },
        "Print the strings passed as parameter");
    rootMenu->Insert(
        "error",
        [](std::ostream&) {
            throw std::logic_error("Error in cmd");
        },
        "Throw an exception in the command handler");
    rootMenu->Insert(
        "reverse", {"string_to_revert"},
        [](std::ostream& out, const string& arg) {
            string copy(arg);
            std::reverse(copy.begin(), copy.end());
            out << copy << "\n";
        },
        "Print the reverse string");
    rootMenu->Insert(
        "add", {"first_term", "second_term"},
        [](std::ostream& out, int x, int y) {
            out << x << " + " << y << " = " << (x + y) << "\n";
        },
        "Print the sum of the two numbers");
    rootMenu->Insert(
        "add",
        [](std::ostream& out, int x, int y, int z) {
            out << x << " + " << y << " + " << z << " = " << (x + y + z) << "\n";
        },
        "Print the sum of the three numbers");
    rootMenu->Insert(
        "sort", {"list of strings separated by space"},
        [](std::ostream& out, std::vector<std::string> data) {
            std::sort(data.begin(), data.end());
            out << "sorted list: ";
            std::copy(data.begin(), data.end(), std::ostream_iterator<std::string>(out, " "));
            out << "\n";
        },
        "Alphabetically sort a list of words");
    rootMenu->Insert(
        "bar",
        [](std::ostream& out, Bar x) { out << "You entered bar: " << x.to_string() << "\n"; },
        "Custom type");
    rootMenu->Insert(
        "complex",
        [](std::ostream& out, std::complex<double> x) { out << "You entered complex : " << x << "\n"; },
        "Print a complex number");
    colorCmd = rootMenu->Insert(
        "color",
        [&](std::ostream& out) {
            out << "Colors ON\n";
            SetColor();
            colorCmd.Disable();
            nocolorCmd.Enable();
        },
        "Enable colors in the cli");
    nocolorCmd = rootMenu->Insert(
        "nocolor",
        [&](std::ostream& out) {
            out << "Colors OFF\n";
            SetNoColor();
            colorCmd.Enable();
            nocolorCmd.Disable();
        },
        "Disable colors in the cli");
    rootMenu->Insert(
        "removecmds",
        [&](std::ostream&) {
            colorCmd.Remove();
            nocolorCmd.Remove();
        },
        "Remove both color and nocolor commands from the menu");

    // a submenu
    // first parameter is the command to enter the submenu
    // second parameter (optional) is the description of the menu in the help
    // third parameter (optional) is the prompt of the menu (default is the name of the command)
    auto subMenu = make_unique<Menu>("sub", "Enter a submenu", "cli-submenu");
    subMenu->Insert(
        "hello",
        [](std::ostream& out) { out << "Hello, submenu world\n"; },
        "Print hello world in the submenu");
    auto subSubMenu = make_unique<Menu>("subsub", "Enter a submenu of second level", "cli-submenu-subsub");
    subSubMenu->Insert(
        "hello",
        [](std::ostream& out) { out << "Hello, subsubmenu world\n"; },
        "Print hello world in the sub-submenu");
    subMenu->Insert(std::move(subSubMenu));
    rootMenu->Insert(std::move(subMenu));

    // create a cli with the given root menu and a persistent storage
    // you must pass to FileHistoryStorage the path of the history file
    // if you don't pass the second argument, the cli will use a VolatileHistoryStorage object that keeps in memory
    // the history of all the sessions, until the cli is shut down.
    cli = make_unique<Cli>(std::move(rootMenu), std::make_unique<FileHistoryStorage>(".cli"));
    // global exit action
    cli->ExitAction([](auto& out) { out << "Goodbye and thanks for all the fish.\n"; });
    // std exception custom handler
    cli->StdExceptionHandler(
        [](std::ostream& out, const std::string& cmd, const std::exception& e) {
            out << "Exception caught in cli handler: "
                << e.what()
                << " handling command: "
                << cmd
                << ".\n";
        });
    // custom handler for unknown commands
    cli->WrongCommandHandler(
        [](std::ostream& out, const std::string& cmd) {
            out << "Unknown command or incorrect parameters: "
                << cmd
                << ".\n";
        });
}

void Repl::start_local_terminal_session() {
    local_session = make_unique<CliLocalTerminalSession>(*cli, scheduler, std::cout, 200);
    local_session->ExitAction(
        [this](auto& out)  // session exit action
        {
            out << "Closing App...\n";
            scheduler.Stop();
        });
}

void Repl::start_telnet_session(int port) {
    telnet_session = make_unique<CliTelnetServer>(*cli, scheduler, port);
    // exit action for all the connections
    telnet_session->ExitAction([](auto& out) { out << "Terminating this session...\n"; });
}

void Repl::start_file_session(std::istream& in, std::ostream& out) {
    file_session = make_unique<CliFileSession>(*cli, in, out);
    file_session->Start();
}
