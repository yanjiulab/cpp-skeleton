#pragma once
#include <chrono>
#include <string>

#include "event.h"

namespace ConfigCpp {

struct Notification {
    Notification(const Event& event, std::string path, std::chrono::steady_clock::time_point time)
        : m_event(event), m_path(std::move(path)), m_time(time) {}

    Event m_event;
    std::string m_path;
    std::chrono::steady_clock::time_point m_time;
};

}  // namespace ConfigCpp