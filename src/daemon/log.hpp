#pragma once

#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
// #include <spdlog/sinks/ostream_sink.h>
#include <spdlog/spdlog.h>

class UserLogger {
  public:
    explicit UserLogger(bool dae_mode = false) {
        console_sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink_->set_level(spdlog::level::debug);
        console_sink_->set_pattern("[%D %H:%M:%S.%e] [%^%L%$] [%t] %@ %v");

        file_sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
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

        if (dae_mode) {
            logger_ = std::make_shared<spdlog::logger>("file_sink", file_sink_);
        } else {
            logger_ = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list({console_sink_, file_sink_}));
        }

        logger_->set_level(spdlog::level::debug);
        logger_->flush_on(spdlog::level::info);
        spdlog::set_default_logger(logger_);
    }

  private:
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink_;
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink_;
    // std::shared_ptr<spdlog::sinks::ostream_sink_mt> os_sink_;
    
    std::shared_ptr<spdlog::logger> logger_;
};