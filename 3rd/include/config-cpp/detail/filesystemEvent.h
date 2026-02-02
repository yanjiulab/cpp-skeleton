#pragma once

#include <chrono>
#include <string>
#include <sys/inotify.h>

namespace ConfigCpp {
struct FileSystemEvent {
    FileSystemEvent() = default;

    FileSystemEvent(int wd, uint32_t mask, std::string path, const std::chrono::steady_clock::time_point &eventTime) : m_wd(wd), m_mask(mask), m_path(std::move(path)), m_eventTime(eventTime) {}

    FileSystemEvent(const FileSystemEvent &rhs) = default;

    FileSystemEvent(FileSystemEvent &&rhs) = default;

    ~FileSystemEvent() = default;

    FileSystemEvent &operator=(const FileSystemEvent &rhs) = default;

    FileSystemEvent &operator=(FileSystemEvent &&rhs) = default;

    int m_wd = 0;
    uint32_t m_mask = 0;
    std::string m_path;
    std::chrono::steady_clock::time_point m_eventTime;
};

}  // namespace ConfigCpp