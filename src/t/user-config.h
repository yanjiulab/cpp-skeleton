#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"
#include "toml.hpp"

// using nlohmann::json;

// Sample user-provided datatype matching ../inputs/json/config.json
//
// See https://github.com/nlohmann/json/blob/develop/README.md for converting to/from arbitrary types

struct UserConfig {
    std::string top_string;
    int top_int;
    bool top_bool;

    std::vector<std::string> top_array;

    std::map<std::string, std::string> top_table;

    std::vector<std::vector<std::string>> nested_array;

    std::map<std::string, std::map<std::string, std::string>> nested_table;

    bool operator==(const UserConfig &rhs) const;
};

std::ostream &operator<<(std::ostream &os, const UserConfig &s);

void to_json(nlohmann::basic_json<> &j, const UserConfig &s);

void from_json(const nlohmann::basic_json<> &j, UserConfig &s);

// Implement specialization of toml::from for struct UserConfig
namespace toml {
template <>
struct from<UserConfig> {
    static UserConfig from_toml(const value &v) {
        UserConfig s;
        s.top_string = find<std::string>(v, "top-string");
        s.top_int = find<int>(v, "top-int");
        s.top_bool = find<bool>(v, "top-bool");

        s.top_array = find<std::vector<std::string>>(v, "top-array");

        s.nested_array = find<std::vector<std::vector<std::string>>>(v, "nested-array");

        s.top_table = find<std::map<std::string, std::string>>(v, "top-table");

        s.nested_table = find<std::map<std::string, std::map<std::string, std::string>>>(v, "nested-table");

        return s;
    }
};
}  // namespace toml