#pragma once

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include <stdexcept>
#include <string>
#include <system_error>

#include "asio/io_context.hpp"
#include "asio/signal_set.hpp"

namespace lynx {
struct dconfig {
    std::string working_dir = "/";
    std::string daemon_name = "lynx";
    std::string output_file = "/tmp/" + daemon_name + ".log";
    // std::string pid_flie = "/tmp/" + daemon_name + ".pid";
    mode_t file_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int file_flags = O_WRONLY | O_CREAT | O_APPEND;
};

class dlog {
  public:
    /**
     * initialize the logger
     * @param daemon_name
     */
    static void init(const std::string& daemon_name) {
        openlog(daemon_name.c_str(), LOG_PID | LOG_NDELAY, LOG_DAEMON);
    }

    /**
     * main logger with priority LOG_X
     * @param message
     * @param priority
     */
    static void log(const std::string& message, std::int32_t priority) {
        syslog(priority, "%s", message.c_str());
    }

    /**
     * debug-level messages
     * @param message
     */
    static void debug(const std::string& message) {
        log(message, LOG_DEBUG);
    }

    /**
     * informational
     * @param message
     */
    static void info(const std::string& message) {
        log(message, LOG_INFO);
    }

    /**
     * normal but significant condition
     * @param message
     */
    static void notice(const std::string& message) {
        log(message, LOG_NOTICE);
    }

    /**
     * warning conditions
     * @param message
     */
    static void warning(const std::string& message) {
        log(message, LOG_WARNING);
    }

    /**
     * error conditions
     * @param message
     */
    static void error(const std::string& message) {
        log(message, LOG_ERR);
    }

    /**
     * critical conditions
     * @param message
     */
    static void critical(const std::string& message) {
        log(message, LOG_CRIT);
    }

    /**
     * action must be taken immediately
     * @param message
     */
    static void alert(const std::string& message) {
        log(message, LOG_ALERT);
    }

    /**
     * system is unusable
     * @param message
     */
    static void emergency(const std::string& message) {
        log(message, LOG_EMERG);
    }

    /**
     * shutdown the logger
     */
    static void shutdown() {
        closelog();
    }

  private:
    static std::string priority_str(std::int32_t priority) {
        switch (priority) {
            case LOG_EMERG:
                return "emergency";
            case LOG_ALERT:
                return "alert";
            case LOG_CRIT:
                return "critical";
            case LOG_ERR:
                return "error";
            case LOG_WARNING:
                return "warning";
            case LOG_NOTICE:
                return "notice";
            case LOG_INFO:
                return "info";
            case LOG_DEBUG:
                return "debug";
            default:
                return "unknown_priority";
        }
    }
};

class Daemon {
  public:
    Daemon(asio::io_context& io_context, const dconfig& cfg)
        : io_context_(io_context), cfg_(cfg) {}

    Daemon(asio::io_context& io_ctx)
        : io_context_(io_ctx), cfg_(get_default_config()) {}

    // 执行守护进程化
    void daemonize() {
        try {
            io_context_.notify_fork(asio::io_context::fork_prepare);

            pid_t pid = fork();
            if (pid < 0) {
                throw std::system_error(errno, std::system_category(), "First fork failed");
            } else if (pid > 0) {
                std::exit(EXIT_SUCCESS);
            }

            if (setsid() < 0) {
                throw std::system_error(errno, std::system_category(), "setsid failed");
            }

            if (chdir(cfg_.working_dir.c_str()) < 0) {
                throw std::system_error(errno, std::system_category(), "chdir failed");
            }

            umask(0);

            pid = fork();
            if (pid < 0) {
                throw std::system_error(errno, std::system_category(), "Second fork failed");
            } else if (pid > 0) {
                std::exit(EXIT_SUCCESS);
            }

            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);

            if (open("/dev/null", O_RDONLY) < 0) {
                throw std::system_error(errno, std::system_category(), "Unable to open /dev/null");
            }

            if (open(cfg_.output_file.c_str(), cfg_.file_flags, cfg_.file_mode) < 0) {
                throw std::system_error(errno, std::system_category(), "Unable to open output file");
            }

            // Also send standard error to the same log file.
            if (dup(1) < 0) {
                throw std::system_error(errno, std::system_category(), "Unable to dup stderr");
            }

            io_context_.notify_fork(asio::io_context::fork_child);

            dlog::init(cfg_.daemon_name);
            syslog(LOG_INFO, "%s started", cfg_.daemon_name.c_str());

        } catch (const std::exception& e) {
            syslog(LOG_ERR, "Failed to daemonize: %s", e.what());
            std::cerr << "Failed to daemonize: " << e.what() << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

  private:
    static dconfig get_default_config() {
        dconfig default_cfg;
        return default_cfg;
    }

    asio::io_context& io_context_;
    const dconfig cfg_;
};
}  // namespace lynx