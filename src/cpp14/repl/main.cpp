
#include "asio.hpp"
#include "repl.hpp"

#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    try {
        // main application that creates an asio io_context and uses it
        asio::io_context io_ctx;

        // REPL setup
        Repl repl(io_ctx);
        
        repl.start_local_terminal_session();
        repl.local_session->ExitAction(
            [&](auto& out) {
                out << "Closing App by Cli...\n";
                // Gracefully shut down other components here
                // ...
                repl.stop();
            });

        repl.start_telnet_session(8888);

        std::ifstream infile("etc/repl.in");
        if (infile.is_open()) {
            std::ofstream outfile("etc/repl.out");
            if (outfile.is_open()) {
                repl.start_file_session(infile, outfile);
            }
        }

        // keep io_context running even if there are no immediate tasks
        auto work = asio::make_work_guard(io_ctx);

        // start the asio io_context
        io_ctx.run();

    } catch (const std::exception& e) {
        std::cerr << "Exception caught in main: " << e.what() << '\n';
    } catch (...) {
        std::cerr << "Unknown exception caught in main.\n";
    }

    return 0;
}