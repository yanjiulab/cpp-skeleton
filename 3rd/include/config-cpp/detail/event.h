#pragma once
#include <sys/inotify.h>

#include <cstdint>
#include <iostream>
#include <type_traits>

namespace ConfigCpp {

enum class Event : std::uint32_t {
    access = IN_ACCESS,
    attrib = IN_ATTRIB,
    close_write = IN_CLOSE_WRITE,
    close_nowrite = IN_CLOSE_NOWRITE,
    close = IN_CLOSE,
    create = IN_CREATE,
    remove = IN_DELETE,
    remove_self = IN_DELETE_SELF,
    modify = IN_MODIFY,
    move_self = IN_MOVE_SELF,
    moved_from = IN_MOVED_FROM,
    moved_to = IN_MOVED_TO,
    move = IN_MOVE,
    open = IN_OPEN,
    is_dir = IN_ISDIR,
    unmount = IN_UNMOUNT,
    q_overflow = IN_Q_OVERFLOW,
    ignored = IN_IGNORED,
    oneshot = IN_ONESHOT,
    all = IN_ALL_EVENTS
};

constexpr Event operator&(Event lhs, Event rhs) {
    return static_cast<Event>(static_cast<std::underlying_type<Event>::type>(lhs) &
                              static_cast<std::underlying_type<Event>::type>(rhs));
}

constexpr Event operator|(Event lhs, Event rhs) {
    return static_cast<Event>(static_cast<std::underlying_type<Event>::type>(lhs) |
                              static_cast<std::underlying_type<Event>::type>(rhs));
}

inline bool containsEvent(const Event& allEvents, const Event& event) {
    return static_cast<std::uint32_t>(event & allEvents) != 0;
}

inline std::ostream& operator<<(std::ostream& stream, const Event& event) {
    std::string maskString;

    if (containsEvent(event, Event::access)) {
        maskString.append("access ");
    }
    if (containsEvent(event, Event::attrib)) {
        maskString.append("attrib ");
    }
    if (containsEvent(event, Event::close_write)) {
        maskString.append("close_write ");
    }
    if (containsEvent(event, Event::close_nowrite)) {
        maskString.append("close_nowrite ");
    }
    if (containsEvent(event, Event::create)) {
        maskString.append("create ");
    }
    if (containsEvent(event, Event::remove)) {
        maskString.append("remove ");
    }
    if (containsEvent(event, Event::remove_self)) {
        maskString.append("remove_self ");
    }
    if (containsEvent(event, Event::modify)) {
        maskString.append("modify ");
    }
    if (containsEvent(event, Event::move_self)) {
        maskString.append("move_self ");
    }
    if (containsEvent(event, Event::moved_from)) {
        maskString.append("moved_from ");
    }
    if (containsEvent(event, Event::moved_to)) {
        maskString.append("moved_to ");
    }
    if (containsEvent(event, Event::open)) {
        maskString.append("open ");
    }
    if (containsEvent(event, Event::is_dir)) {
        maskString.append("is_dir ");
    }
    if (containsEvent(event, Event::unmount)) {
        maskString.append("unmount ");
    }
    if (containsEvent(event, Event::q_overflow)) {
        maskString.append("q_overflow ");
    }
    if (containsEvent(event, Event::close)) {
        maskString.append("close ");
    }
    if (containsEvent(event, Event::ignored)) {
        maskString.append("ignored ");
    }
    if (containsEvent(event, Event::oneshot)) {
        maskString.append("oneshot ");
    }

    stream << maskString;
    return stream;
}



}  // namespace ConfigCpp