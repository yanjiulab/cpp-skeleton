#pragma once

#include <string>
#include <vector>

namespace ConfigCpp {

struct Value {
    enum ValueType { BOOL,
                     INT,
                     DOUBLE,
                     STRING };
    double m_double;
    int m_int;
    std::string m_key;
    std::string m_string;
    ValueType m_type;
    bool m_bool;

    Value(const Value &rhs) = default;

    Value(Value &&rhs) = default;

    Value(std::string key, const bool &boolVal)
        : m_double(0), m_int(0), m_key(std::move(key)), m_type(BOOL), m_bool(boolVal) {}

    Value(std::string key, const int &intVal)
        : m_double(0), m_int(intVal), m_key(std::move(key)), m_type(INT), m_bool(false) {}

    Value(std::string key, const double &doubleVal)
        : m_double(doubleVal), m_int(0), m_key(std::move(key)), m_type(DOUBLE), m_bool(false) {}

    Value(std::string key, std::string stringVal)
        : m_double(0), m_int(0), m_key(std::move(key)), m_string(std::move(stringVal)), m_type(STRING), m_bool(false) {}

    Value(std::string key, const char *stringVal)
        : m_double(0), m_int(0), m_key(std::move(key)), m_string(stringVal), m_type(STRING), m_bool(false) {}

    ~Value() = default;

    Value &operator=(const Value &rhs) = default;

    Value &operator=(Value &&rhs) = default;
};

using Values = std::vector<Value>;

}  // namespace ConfigCpp
