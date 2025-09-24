#include <cli/cli.h>
#include <cli/clifilesession.h>
#include <cli/clilocalsession.h>
#include <cli/standaloneasioremotecli.h>
#include <cli/standaloneasioscheduler.h>

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

istream& operator>>(istream& in, Bar& p) {
    in >> p.value;
    return in;
}

class UserInterface {
  public:
    explicit UserInterface(IoContext& iocontext)
        : scheduler(iocontext) {
        auto rootMenu = make_unique<Menu>("cli");

        rootMenu->Insert(
            "hello",
            [](std::ostream& out) { out << "Hello, world\n"; },
            "Print hello world");
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

        localSession = make_unique<CliLocalTerminalSession>(*cli, scheduler, std::cout, 200);
        localSession->ExitAction(
            [this](auto& out)  // session exit action
            {
                out << "Closing App...\n";
                scheduler.Stop();
            });

        telnetSession = make_unique<CliTelnetServer>(*cli, scheduler, 5000);
        // exit action for all the connections
        telnetSession->ExitAction([](auto& out) { out << "Terminating this session...\n"; });

        fileSession = make_unique<CliFileSession>(*cli, infile, outfile);
        fileSession->Start();
    }

    // UserInterface(IoContext& iocontext, std::string& in, std::string& out) : scheduler(iocontext),
    //                                                                          infile(in),
    //                                                                          outfile(out) {
    // }

    int set_input_file(std::string i) {
        std::ifstream infile(i);
        if (!infile) {
            std::cerr << "File input.txt not found in current directory!\n";
            return -1;
        }
    };
    void set_output_file(std::string o) {
        std::ofstream outfile(o);
        if (!outfile) {
            std::cerr << "Can't write file output.txt in the current directory!\n";
            // return 1;
        }
    };

  private:
    StandaloneAsioScheduler scheduler;
    unique_ptr<Cli> cli;
    unique_ptr<CliLocalTerminalSession> localSession;
    unique_ptr<CliTelnetServer> telnetSession;
    unique_ptr<CliFileSession> fileSession;
    std::ifstream infile;
    std::ofstream outfile;
    CmdHandler colorCmd;
    CmdHandler nocolorCmd;
};