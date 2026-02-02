#pragma once

#include "tomlHandler.h"
#include "util.h"

namespace ConfigCpp {

inline TomlHandler::TomlHandler(const std::string &data, const Values &defaults, const Values &cmdLineArgs) {
    try {
        std::istringstream streamData(data, std::ios_base::binary | std::ios_base::in);

        m_toml = toml::parse(streamData, "string");

    } catch (...) {
        throw std::runtime_error("Invalid toml received");
    }
    // For defaults, only integrate default values where the specific key isn't
    // part of the config data already
    for (const auto &def : defaults) {
        toml::value value;
        if (!GetNode(def.m_key, value)) {
            AddDefaultNode(def);
        }
    }
    // For command-line arguments these values supercede any values read from
    // config data
    for (const auto &arg : cmdLineArgs) {
        AddDefaultNode(arg);
    }
}

inline bool TomlHandler::IsSet(const std::string &key) const {
    toml::value value;
    return GetNode(key, value);
}

inline bool TomlHandler::GetBool(const std::string &key) const {
    toml::value value;
    try {
        if (GetNode(key, value)) {
            if (value.is_boolean()) {
                return value.as_boolean();
            }
        }
    } catch (...) {
    }
    return false;
}

inline int TomlHandler::GetInt(const std::string &key) const {
    toml::value value;
    try {
        if (GetNode(key, value)) {
            if (value.is_integer()) {
                return static_cast<int>(value.as_integer());
            } else if (value.is_boolean()) {
                return value.as_boolean() ? 1 : 0;
            } else if (value.is_floating()) {
                return static_cast<int>(value.as_floating());
            }
        }
    } catch (...) {
    }
    return 0;
}

inline double TomlHandler::GetDouble(const std::string &key) const {
    toml::value value;
    try {
        if (GetNode(key, value)) {
            if (value.is_floating()) {
                return value.as_floating();
            } else if (value.is_integer()) {
                return static_cast<double>(value.as_integer());
            }
        }
    } catch (...) {
    }
    return 0.0;
}

inline std::string TomlHandler::GetString(const std::string &key) const {
    toml::value value;
    try {
        if (GetNode(key, value)) {
            if (value.is_string()) {
                return value.as_string();
            }
        }
    } catch (...) {
    }
    return "";
}

inline bool TomlHandler::GetNode(const std::string &key, toml::value &value) const {
    auto keys = split(key, '.');
    auto cur = m_toml;
    for (const auto &k : keys) {
        try {
            auto next = toml::find(cur, k);
            cur = next;
        } catch (...) {
            return false;
        }
    }
    value = cur;
    return true;
}

inline bool TomlHandler::AddDefaultNode(const Value &def) {
    auto keys = split(def.m_key, '.');
    if (keys.size() == 1) {
        auto key = keys[0];

        switch (def.m_type) {
            case Value::BOOL:
                m_toml[key] = def.m_bool;
                break;
            case Value::INT:
                m_toml[key] = def.m_int;
                break;
            case Value::DOUBLE:
                m_toml[key] = def.m_double;
                break;
            case Value::STRING:
                m_toml[key] = def.m_string;
                break;
        }
        return true;
    }
    return false;
}

inline std::string TomlHandler::GetConfig() const {
    return toml::format(m_toml);
}

inline void TomlHandler::SetBool(const std::string &key, const bool &boolVal) {
    auto keys = split(key, '.');
    auto cur = &m_toml;
    for (size_t i = 0; i < keys.size() - 1; ++i) {
        const std::string &k = keys[i];
        if (!cur->is_table() || !cur->contains(k)) {
            (*cur)[k] = toml::table{};
        }
        cur = &((*cur)[k]);
    }
    auto &k = keys.back();
    (*cur)[k] = boolVal;
}

inline void TomlHandler::SetInt(const std::string &key, const int &intVal) {
    auto keys = split(key, '.');
    auto cur = &m_toml;
    for (size_t i = 0; i < keys.size() - 1; ++i) {
        const std::string &k = keys[i];
        if (!cur->is_table() || !cur->contains(k)) {
            (*cur)[k] = toml::table{};
        }
        cur = &((*cur)[k]);
    }
    auto &k = keys.back();
    (*cur)[k] = intVal;
}

inline void TomlHandler::SetDouble(const std::string &key, const double &doubleVal) {
    auto keys = split(key, '.');
    auto cur = &m_toml;
    for (size_t i = 0; i < keys.size() - 1; ++i) {
        const std::string &k = keys[i];
        if (!cur->is_table() || !cur->contains(k)) {
            (*cur)[k] = toml::table{};
        }
        cur = &((*cur)[k]);
    }
    auto &k = keys.back();
    (*cur)[k] = doubleVal;
}

inline void TomlHandler::SetString(const std::string &key, const std::string &stringVal) {
    auto keys = split(key, '.');
    auto cur = &m_toml;
    for (size_t i = 0; i < keys.size() - 1; ++i) {
        const std::string &k = keys[i];
        if (!cur->is_table() || !cur->contains(k)) {
            (*cur)[k] = toml::table{};
        }
        cur = &((*cur)[k]);
    }
    auto &k = keys.back();
    (*cur)[k] = stringVal;
}

}  // Namespace ConfigCpp