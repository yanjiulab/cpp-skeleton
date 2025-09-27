#pragma once

#include <syslog.h>
#include <unistd.h>

#include <cstdio>
#include <string>

#include "asio.hpp"

struct dconfig {
    std::string daemon_name = "user-daemon";
    std::string working_dir = "/";                             // 工作目录
    std::string output_file = "/tmp/asio.daemon.log";          // 标准输出日志路径
    mode_t file_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;  // 日志文件权限
    int file_flags = O_WRONLY | O_CREAT | O_APPEND;            // 日志文件打开模式
};

class dlog {
  public:
    /**
     * initialize the logger
     * @param daemon_name
     */
    static void init(const std::string& daemon_name) {
        openlog(daemon_name.c_str(), LOG_PID, LOG_DAEMON);
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

class UserDaemon {
  public:
    UserDaemon(std::shared_ptr<asio::io_context> io_ctx, const struct dconfig& cfg)
        : m_io_context(std::move(io_ctx)) {
        // Inform the io_context that we are about to become a daemon. The
        // io_context cleans up any internal resources, such as threads, that may
        // interfere with forking.
        m_io_context->notify_fork(asio::io_context::fork_prepare);

        // Fork the process and have the parent exit. If the process was started
        // from a shell, this returns control to the user. Forking a new process is
        // also a prerequisite for the subsequent call to setsid().
        if (m_pid = fork()) {
            if (m_pid > 0) {
                std::exit(EXIT_SUCCESS);
            } else {
                dlog::error("First fork failed: %m");
                std::exit(EXIT_FAILURE);
            }
        }

        // Make the process a new session leader. This detaches it from the
        // terminal.
        setsid();

        // A process inherits its working directory from its parent. This could be
        // on a mounted filesystem, which means that the running daemon would
        // prevent this filesystem from being unmounted. Changing to the root
        // directory avoids this problem.
        chdir(cfg.working_dir.c_str());

        // The file mode creation mask is also inherited from the parent process.
        // We don't want to restrict the permissions on files created by the
        // daemon, so the mask is cleared.
        umask(0);

        dlog::init(cfg.daemon_name);

        // A second fork ensures the process cannot acquire a controlling terminal.
        if (m_pid = fork()) {
            if (m_pid > 0) {
                std::exit(EXIT_SUCCESS);
            } else {
                dlog::error("Second fork failed: %m");
                std::exit(EXIT_FAILURE);
            }
        }

        // Close the standard streams. This decouples the daemon from the terminal
        // that started it.
        close(0);
        close(1);
        close(2);

        // We don't want the daemon to have any standard input.
        if (open("/dev/null", O_RDONLY) < 0) {
            dlog::error("Unable to open /dev/null: %m");
            std::exit(EXIT_FAILURE);
        }

        // Send standard output to a log file.
        const char* output = cfg.output_file.c_str();
        if (open(output, cfg.file_flags, cfg.file_mode) < 0) {
            dlog::error("Unable to open output file: " + cfg.output_file + "%m");
            std::exit(EXIT_FAILURE);
        }

        // Also send standard error to the same log file.
        if (dup(1) < 0) {
            dlog::error("Unable to dup output descriptor: %m");
            std::exit(EXIT_FAILURE);
        }

        // Inform the io_context that we have finished becoming a daemon. The
        // io_context uses this opportunity to create any internal file descriptors
        // that need to be private to the new process.
        m_io_context->notify_fork(asio::io_context::fork_child);

        // The io_context can now be used normally.
        // context.run();
        dlog::info("Daemon started");
    }

    ~UserDaemon() {
        dlog::shutdown();
        // Terminate the child process when the daemon completes (loop stopped)
        // note that calling std::exit() inside the run function will not call dtor.
        std::exit(EXIT_SUCCESS);
    }

    pid_t get_pid() const noexcept { return m_pid; }

  private:
    std::shared_ptr<asio::io_context> m_io_context;
    // struct dconfig& m_cfg;
    pid_t m_pid;
};
