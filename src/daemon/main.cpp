
#include "asio.hpp"
#include "config.hpp"
#include "daemon.hpp"
#include "log.hpp"
#include "repl.hpp"
#include "toml.hpp"

// sudo apt-get install libdw-dev
// target_link_libraries(your_target dw)
// #define BACKWARD_HAS_DW 1
// #include "backward.hpp"
// namespace backward {
// backward::SignalHandling sh;
// }

int main(int argc, char** argv) {
    try {
        // 初始化命令行参数
        UserConfig cfg(argc, argv);
        // usage example
        std::cout << "cli: " << cfg.cli["--pi"]->as<double>() << std::endl;
        std::cout << "variable: " << cfg.data.pi << std::endl;
        std::cout << "toml: " << toml::find<double>(cfg.root, "pi") << std::endl;
        cfg.WriteConfig(false);

        // 初始化日志
        UserLogger log(cfg.data.dae);
        spdlog::info("Welcome to spdlog!")({{"key1", 10}, {"k2", "val2"}});
        spdlog::error("Some error message with arg: {}", 1);
        spdlog::warn("Easy padding in numbers like {:08d}", 12);
        spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
        spdlog::info("Support for floats {:03.2f}", 1.23456);
        spdlog::info("Positional args are {1} {0}..", "too", "supported");
        spdlog::info("{:<30}", "left aligned");

        // main application that creates an asio io_context and uses it
        asio::io_context iocontext;
        asio::steady_timer timer(iocontext, std::chrono::seconds(5));
        timer.async_wait([&timer](const error_code&) {
            cout << "Timer expired!\n";
        });

        // REPL setup
        UserREPL repl(iocontext);
        if (!cfg.data.dae) {
            repl.StartLocalSession();
        }
        repl.StartTelnetSession(8888);
        // std::ifstream infile("etc/repl.in");
        // std::ofstream outfile("etc/repl.out");
        // repl.StartFileSession(infile, outfile);

        // Register signal handlers so that the daemon may be shut down. You may
        // also want to register for other signals, such as SIGHUP to trigger a
        // re-read of a configuration file.
        asio::signal_set signals(iocontext, SIGINT, SIGTERM);
        signals.async_wait(
            [&](std::error_code /*ec*/, int /*signo*/) {
                iocontext.stop();
            });

        if (cfg.data.dae) {
            auto io_ptr = std::shared_ptr<asio::io_context>(
                &iocontext,
                [](asio::io_context*) {}  // 空删除器：什么都不做
            );

            // auto io_ptr = std::make_shared<asio::io_context>(std::ref(iocontext));
            struct dconfig dcfg;
            UserDaemon dae(io_ptr, dcfg);
            // spdlog::info("daemon running in: {}", dae.get_pid());
        }

        // start the asio io_context
        // auto work = asio::make_work_guard(iocontext);
        iocontext.run();

        std::cout << "not here" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        cerr << "Exception caught in main: " << e.what() << '\n';
    } catch (...) {
        cerr << "Unknown exception caught in main.\n";
    }

    std::cout << "not here" << std::endl;

    return 0;
}