#pragma once

#include <iostream>
#include <memory>
#include <mutex>

class LoggerConfig {
  public:
    static void Initialize();
    // static void InitializeFromFile(const std::string& config_file);
    // static void Shutdown();

    // static void SetGlobalLevel(spdlog::level::level_enum level);

    static void AddConsoleSink();
    static void AddStreamSink(std::ostream &os);
    static void RemoveAllStreamSink();
    // static void AddFileSink(const std::string& filename, bool truncate = true);
    // static void AddRotatingFileSink(const std::string& filename, size_t max_size, size_t max_files);
    // static bool RemoveSinkByPattern(const std::string& pattern);

  private:
    // static void SetupDefaultSinks();
    // static void SetupFromConfig(const std::string& config_file);

    static std::mutex sink_mutex_;
};