#pragma once

#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

class UserLogger {
  public:
    explicit UserLogger() {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);
        console_sink->set_pattern("[%D %H:%M:%S.%e] [%^%L%$] [%t] [%&] %@ %v");

        auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
            "logs/daily.log", 00, 00, false);
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
        file_sink->set_level(spdlog::level::debug);

        m_logger = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({console_sink, file_sink}));
        m_logger->set_level(spdlog::level::debug);
        m_logger->flush_on(spdlog::level::info);
        spdlog::set_default_logger(m_logger);
    }

  private:
    std::shared_ptr<spdlog::logger> m_logger;
};