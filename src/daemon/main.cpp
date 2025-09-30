
#include "asio.hpp"
#include "config.hpp"
#include "daemon.hpp"
#include "log.hpp"
#include "repl.hpp"
#include "rest_server.hpp"
#include "toml.hpp"

// sudo apt-get install libdw-dev
// target_link_libraries(your_target dw)
// #define BACKWARD_HAS_DW 1
// #include "backward.hpp"
// namespace backward {
// backward::SignalHandling sh;
// }

void handle_signal(const std::error_code& ec, int signal_number,
                   asio::signal_set& signals) {
    if (!ec) {
        std::cout << "Received signal: " << signal_number << std::endl;
        // 根据信号值区分不同信号并处理
        switch (signal_number) {
            case SIGHUP:
                std::cout << "Received SIGHUP (hangup)" << std::endl;
                // 可以在这里处理配置文件重新加载等逻辑
                break;
            case SIGUSR1:
                break;
            case SIGUSR2:
                break;
        }

        // Re-arm the signal handler to catch SIGHUP again
        signals.async_wait([&signals](const asio::error_code& ec, int signal_number) {
            handle_signal(ec, signal_number, signals);
        });
    }
}

void print(const std::error_code& ec, asio::steady_timer* t, int* count) {
    if (!ec) {
        if (*count) {
            // std::cout << *count << std::endl;
            spdlog::info("count: {}", *count);
            --(*count);

            t->expires_at(t->expiry() + asio::chrono::seconds(1));
            t->async_wait(std::bind(print, asio::placeholders::error, t, count));
        }
    } else if (ec == asio::error::operation_aborted) {
        std::cout << "Timer was cancelled\n";
    } else {
        std::cout << "Timer error: " << ec.message() << "\n";
    }
}

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
        asio::io_context io_ctx;

        // Timers
        int count = 5;
        // asio::steady_timer t(io_ctx, asio::chrono::seconds(1));
        asio::steady_timer t(io_ctx);
        t.expires_at(std::chrono::steady_clock::now());
        t.async_wait(std::bind(print, asio::placeholders::error, &t, &count));

        asio::steady_timer timer(io_ctx, std::chrono::milliseconds(1500));
        timer.async_wait([&timer, &t, &count](const std::error_code&) {
            // std::cout << "Steady Timer expired!\n";
            spdlog::info("Steady Timer expired!")({{"left", count}});

            t.cancel();
            count = 100;
            t.expires_after(std::chrono::seconds(2));
            t.async_wait(std::bind(print, asio::placeholders::error, &t, &count));
        });

        // Http REST API 服务器
        RestServer rest(io_ctx, 8080, "0.0.0.0");
        rest.SetupRoutes();
        rest.server().async_start();

        // REPL setup
        UserREPL repl(io_ctx);
        if (!cfg.data.dae) {
            repl.StartLocalSession();
            repl.local_session->ExitAction(
                [&](auto& out) {
                    out << "Closing App by Cli...\n";
                    rest.server().stop();
                    repl.Stop();
                });
        }
        repl.StartTelnetSession(8888);
        std::ifstream infile("etc/repl.in");
        if (infile.is_open()) {
            std::ofstream outfile("etc/repl.out");
            if (outfile.is_open()) {
                repl.StartFileSession(infile, outfile);
            }
        }

        // Register signal handler so that the daemon may be shut down.
        asio::signal_set exit_signals(io_ctx, SIGINT, SIGTERM);
        exit_signals.async_wait([&](std::error_code ec, int signo) {
            // std::thread([&] { rest.server().stop(); }).detach();
            std::cout << "Closing App by signal...\n";
            rest.server().stop();
            repl.Stop();
        });

        // Register other signal handler.
        asio::signal_set user_signals(io_ctx, SIGHUP, SIGUSR1, SIGUSR2);
        user_signals.async_wait([&user_signals](const std::error_code& ec, int signal_number) {
            handle_signal(ec, signal_number, user_signals);
        });

        // Prepare daemon
        UserDaemon dae(io_ctx);
        if (cfg.data.dae) {
            dae.Daemonize();
            std::cout << "daemon start: " << getpid() << std::endl;
        }

        // start the asio io_context
        // auto work = asio::make_work_guard(io_ctx);
        io_ctx.run();

    } catch (const std::exception& e) {
        std::cerr << "Exception caught in main: " << e.what() << '\n';
    } catch (...) {
        std::cerr << "Unknown exception caught in main.\n";
    }

    return 0;
}