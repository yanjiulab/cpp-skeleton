#include "repl.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "log.hpp"

using namespace tabulate;
using Row_t = Table::Row_t;
using namespace cli;
using namespace std;

namespace lynx {

istream& operator>>(istream& in, Bar& p) {
    in >> p.value;
    return in;
}

Repl::Repl(IoContext& iocontext) : scheduler(iocontext) {
    auto rootMenu = make_unique<Menu>("cli");

    rootMenu->Insert(
        "hello",
        [](std::ostream& out) { out << "Hello, world\n"; },
        "Print hello world");
    rootMenu->Insert(
        "monitor",
        [](std::ostream& out) {
            LoggerConfig::AddStreamSink(out);
        },
        "enter monitor mode");
    rootMenu->Insert(
        "nomonitor",
        [](std::ostream& out) {
            LoggerConfig::RemoveAllStreamSink();
        },
        "exit monitor mode");
    rootMenu->Insert(
        "hello_everysession",
        [](std::ostream&) { Cli::cout() << "Hello, everybody" << std::endl; },
        "Print hello everybody on all open sessions");
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

    rootMenu->Insert(
        "table",
        [&](std::ostream& out) {
            Table movies;
            movies.add_row(Row_t{"S/N", "Movie Name", "Director", "Estimated Budget", "Release Date"});
            movies.add_row(Row_t{"tt1979376", "Toy Story 4", "Josh Cooley", "$200,000,000", "21 June 2019"});
            movies.add_row(Row_t{"tt3263904", "Sully", "Clint Eastwood", "$60,000,000", "9 September 2016"});
            movies.add_row(
                {"tt1535109", "Captain Phillips", "Paul Greengrass", "$55,000,000", " 11 October 2013"});

            // center align 'Director' column
            movies.column(2).format().font_align(FontAlign::center);

            // right align 'Estimated Budget' column
            movies.column(3).format().font_align(FontAlign::right);

            // right align 'Release Date' column
            movies.column(4).format().font_align(FontAlign::right);

            // center-align and color header cells
            for (size_t i = 0; i < 5; ++i) {
                movies[0][i]
                    .format()
                    .font_color(Color::yellow)
                    .font_align(FontAlign::center)
                    .font_style({FontStyle::bold});
            }

            out << movies << std::endl;
        },
        "Show table");

    // a submenu
    // first parameter is the command to enter the submenu
    // second parameter (optional) is the description of the menu in the help
    // third parameter (optional) is the prompt of the menu (default is the name of the command)
    auto subMenu = make_unique<Menu>("sub", "Enter a submenu", "cli-submenu");
    subMenu->Insert(
        "hello",
        [](std::ostream& out) { out << "Hello, submenu world\n"; },
        "Print hello world in the submenu");
    subMenu->Insert(
        "demo",
        [](std::ostream& out) { out << "This is a sample!\n"; },
        "Print a demo string");

    auto subSubMenu = make_unique<Menu>("subsub", "Enter a submenu of second level", "cli-submenu-subsub");
    subSubMenu->Insert(
        "hello",
        [](std::ostream& out) { out << "Hello, subsubmenu world\n"; },
        "Print hello world in the sub-submenu");
    subMenu->Insert(std::move(subSubMenu));

    rootMenu->Insert(std::move(subMenu));

    cli = make_unique<Cli>(std::move(rootMenu));
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
}

void Repl::StartLocalSession() {
    local_session = make_unique<CliLocalTerminalSession>(*cli, scheduler, std::cout, 200);
    local_session->ExitAction(
        [this](auto& out)  // session exit action
        {
            out << "Closing App...\n";
            scheduler.Stop();
        });
}

void Repl::StartTelnetSession(int port) {
    telnet_session = make_unique<CliTelnetServer>(*cli, scheduler, port);
    // exit action for all the connections
    telnet_session->ExitAction([](auto& out) { out << "Terminating this session...\n"; });
}

void Repl::StartFileSession(std::istream& in, std::ostream& out) {
    file_session = make_unique<CliFileSession>(*cli, in, out);
    file_session->Start();
}
}  // namespace lynx