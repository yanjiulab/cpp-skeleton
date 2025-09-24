#include "log.hpp"

#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

void init_logger() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);
    console_sink->set_pattern("[%D %H:%M:%S.%e] [%^%L%$] [%t] %@ %v");

    auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        "logs/daily.log", 00, 00, false);
    file_sink->set_pattern("[%D %H:%M:%S.%e] [%L] [%t] %v");
    file_sink->set_level(spdlog::level::debug);

    spdlog::logger logger("multi_sink", {console_sink, file_sink});
    logger.set_level(spdlog::level::debug);
    logger.flush_on(spdlog::level::info);
    spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));
}

void shutdown_logger() {
    spdlog::shutdown();
}
