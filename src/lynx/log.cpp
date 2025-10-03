#include "log.hpp"

#include "spdlog/pattern_formatter.h"
// #include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/ostream_sink.h"
// #include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace lynx {
std::mutex LoggerConfig::sink_mutex_;

void LoggerConfig::Initialize() {
    std::lock_guard<std::mutex> lock(sink_mutex_);

    try {
        // default sink
        auto file_sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
            "logs/daily.log", 00, 00, false);
        file_sink_->set_formatter(spdlog::details::make_unique<spdlog::json_formatter>());
        file_sink_->set_populators(
            spdlog::details::make_unique<spdlog::populators::date_time_populator>(),
            spdlog::details::make_unique<spdlog::populators::level_populator>(),
            spdlog::details::make_unique<spdlog::populators::thread_id_populator>(),
            spdlog::details::make_unique<spdlog::populators::src_loc_populator>(),
            spdlog::details::make_unique<spdlog::populators::pid_populator>(),
            spdlog::details::make_unique<spdlog::populators::thread_id_populator>(),
            spdlog::details::make_unique<spdlog::populators::timestamp_populator>(),
            spdlog::details::make_unique<spdlog::populators::message_populator>());
        file_sink_->set_level(spdlog::level::debug);

        auto logger_ = std::make_shared<spdlog::logger>("default_sink", file_sink_);
        logger_->set_level(spdlog::level::debug);
        logger_->flush_on(spdlog::level::info);

        spdlog::set_default_logger(logger_);
        spdlog::info("Global logger initialized successfully");
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        throw;
    }
}

void LoggerConfig::AddConsoleSink() {
    std::lock_guard<std::mutex> lock(sink_mutex_);
    try {
        auto console_sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink_->set_level(spdlog::level::debug);
        console_sink_->set_pattern("[%D %H:%M:%S.%e] [%^%L%$] [%t] %@ %v");
        spdlog::default_logger()->sinks().push_back(console_sink_);
    } catch (const spdlog::spdlog_ex& ex) {
        spdlog::error("Failed to add console sink: {}", ex.what());
    }
}

void LoggerConfig::AddStreamSink(std::ostream& os) {
    std::lock_guard<std::mutex> lock(sink_mutex_);
    try {
        auto os_sink_ = std::make_shared<spdlog::sinks::ostream_sink_mt>(os);
        os_sink_->set_pattern("[%D %H:%M:%S.%e] [%^%L%$] [%t] %@ %v");
        spdlog::default_logger()->sinks().push_back(os_sink_);
    } catch (const spdlog::spdlog_ex& ex) {
        spdlog::error("Failed to add ostream sink: {}", ex.what());
    }
}

void LoggerConfig::RemoveAllStreamSink() {
    // spdlog::default_logger()->sinks().pop_back();
    auto sinks = spdlog::default_logger()->sinks();
    sinks.erase(
        std::remove_if(sinks.begin(), sinks.end(), [](const std::shared_ptr<spdlog::sinks::sink>& sink) {
            return std::dynamic_pointer_cast<spdlog::sinks::ostream_sink_mt>(sink) != nullptr;
        }),
        sinks.end());
    spdlog::default_logger()->sinks() = sinks;
}

// void LoggerConfig::AddFileSink(const std::string& filename, bool truncate) {
//     std::lock_guard<std::mutex> lock(sink_mutex_);

//     try {
//         auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, truncate);
//         file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");

//         spdlog::default_logger()->sinks().push_back(file_sink);
//         spdlog::info("Added file sink: {}", filename);

//     } catch (const spdlog::spdlog_ex& ex) {
//         spdlog::error("Failed to add file sink {}: {}", filename, ex.what());
//     }
// }
}  // namespace lynx