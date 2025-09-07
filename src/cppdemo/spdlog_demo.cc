#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"

int spdlog_demo() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);
    console_sink->set_pattern("[multi_sink_example] [%^%l%$] %v");
    
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
    file_sink->set_level(spdlog::level::trace);

    file_sink->set_formatter(spdlog::details::make_unique<spdlog::json_formatter>());
    file_sink->set_populators(
        spdlog::details::make_unique<spdlog::populators::date_time_populator>(),
        spdlog::details::make_unique<spdlog::populators::level_populator>(),
        spdlog::details::make_unique<spdlog::populators::thread_id_populator>(),
        spdlog::details::make_unique<spdlog::populators::src_loc_populator>(),
        spdlog::details::make_unique<spdlog::populators::pid_populator>(),
        spdlog::details::make_unique<spdlog::populators::thread_id_populator>(),
        spdlog::details::make_unique<spdlog::populators::timestamp_populator>(),
        spdlog::details::make_unique<spdlog::populators::message_populator>());


    auto logger = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({console_sink, file_sink}));
    spdlog::set_default_logger(std::move(logger));
    // spdlog::logger logger("multi_sink", {console_sink, file_sink});
    // logger.set_level(spdlog::level::debug);
    // logger.warn("this should appear in both console and file");
    // logger.info("this message should not appear in the console, only in the file");

    spdlog::info("This is an info message with custom populators");

    SPDLOG_INFO("my fuck");
    spdlog::info("Welcome to spdlog {}!", SPDLOG_VERSION)({{"key1", 0}, {"kye", "11"}});
    spdlog::error("Some error message with arg: {}", 1);

    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");

    spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
    spdlog::debug("This message should be displayed..");

    // change log pattern
    // spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    spdlog::debug("This message should be displayed..");
    // spdlog::info("{}", SPDLOG_ACTIVE_LEVEL);

    // Compile time log levels
    // Note that this does not change the current log level, it will only
    // remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("Some debug message");

    return 0;
}