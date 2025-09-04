#include <variant>
#include <iomanip>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/mdc.h"

namespace {
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;
}  // namespace

struct SimpleJSON {
    using json_val = std::variant<std::int64_t, int, double, std::string, bool>;
    std::unordered_map<std::string, json_val> members;

    SimpleJSON(std::initializer_list<std::pair<const std::string, json_val>> il) : members{il} {}

    template <typename OStream>
    friend OStream &operator<<(OStream &os, const SimpleJSON &j) {
        for (const auto &kv : j.members) {
            os << ", " << std::quoted(kv.first) << ":";
            std::visit(overloaded{
                           [&](std::int64_t arg) { os << arg; },
                           [&](int arg) { os << arg; },
                           [&](double arg) { os << arg; },
                           [&](const std::string &arg) { os << std::quoted(arg); },
                           [&](bool arg) { os << (arg ? "true" : "false"); }},
                       kv.second);
        }
        return os;
    }
};

int spdlog_demo() {
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");

    spdlog::set_level(spdlog::level::debug);  // Set global log level to debug
    spdlog::debug("This message should be displayed..");

    // change log pattern
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
    spdlog::debug("This message should be displayed..");

    // Compile time log levels
    // Note that this does not change the current log level, it will only
    // remove (depending on SPDLOG_ACTIVE_LEVEL) the call on the release code.
    SPDLOG_TRACE("Some trace message with param {}", 42);
    SPDLOG_DEBUG("Some debug message");

    using J = SimpleJSON;
    spdlog::set_pattern(
        "{\"timestamp\":\"%Y-%m-%dT%H:%M:%S.%e%z\",\"logger\":\"%n\",\"log_"
        "level\":\"%l\",\"process_id\":%P,\"thread_id\":%t %v}");
    // spdlog::mdc::put("mother", "fuck");
    
    spdlog::info("{} {}", J({{"key1", "value1"}, {"key2", true}, {"key3", 99}, {"key4", 1.2}}));

    return 0;
}