
#include <cstdlib>

#include "CLI11.hpp"
#include "config.h"
#include "spdlog/fmt/fmt.h"

int setup_cli_args(int argc, char** argv) {

    CLI::App app("net daemon");

    app.add_flag_function("-v,--version", [](int count) {
        if (count >= 1) {
            const std::string v1 = fmt::format("{}.{}.{}",
                                        proj_VERSION_MAJOR,
                                        proj_VERSION_MINOR,
                                        proj_VERSION_PATCH);
            std::cout << "Version: " << v1 << std::endl;
        }
        if (count >= 2) {
            const std::string v2 = fmt::format("{} {}",
                                        __DATE__,
                                        __TIME__);
            std::cout << "Compile time: " << v2 << std::endl;
        }
        if (count >= 3) {
            const std::string v3;
            std::cout << "Program MD5: " << v3 << std::endl;
        }
        std::exit(0); }, "Display program version information and exit");

    bool running_daemon{false};
    // app.add_flag("-d,--daemon", running_daemon, "Running program in daemon mode");
    // app.add_flag_function

    // int port{0};
    // app.add_flag("-p,--port", port, "Program telnet port");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    }
}

int main(int argc, char** argv) {
    
    // setup_cli_args(argc, argv);

    std::cout << "not here" << std::endl;

    return 0;
}