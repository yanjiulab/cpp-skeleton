#pragma once

#include <iostream>
#include <memory>
#include <mutex>

namespace lynx {
class LoggerConfig {
  public:
    static void init();
    // static void init_from_file(const std::string& config_file);
    // static void Shutdown();

    // static void SetGlobalLevel(spdlog::level::level_enum level);

    static void add_console_sink();
    static void add_stream_sink(std::ostream &os);
    static void remove_all_stream_sink();
    // static void AddFileSink(const std::string& filename, bool truncate = true);
    // static void AddRotatingFileSink(const std::string& filename, size_t max_size, size_t max_files);
    // static bool RemoveSinkByPattern(const std::string& pattern);

  private:
    // static void SetupDefaultSinks();
    // static void SetupFromConfig(const std::string& config_file);

    static std::mutex sink_mutex_;
};
}  // namespace lynx