#include "user-config.h"

bool UserConfig::operator==(const UserConfig &rhs) const {
    return (top_string == rhs.top_string &&
            top_int == rhs.top_int &&
            top_bool == rhs.top_bool &&
            top_array == rhs.top_array &&
            top_table == rhs.top_table &&
            nested_array == rhs.nested_array &&
            nested_table == rhs.nested_table);
}

std::ostream &operator<<(std::ostream &os, const UserConfig &s) {
    os << "top_string: " << s.top_string << "\n"
       << "top_int: " << s.top_int << "\n"
       << "top_bool: " << s.top_bool << "\n"
       << "top_array:\n";
    for (const auto &item : s.top_array) {
        os << "  " << item << "\n";
    }
    os << "top_table:\n";
    for (const auto &item : s.top_table) {
        os << "  " << item.first << ": " << item.second << "\n";
    }
    for (const auto &item : s.nested_array) {
        os << "  ListElt" << "\n";
        for (const auto &subitem : item) {
            os << "    " << subitem << "\n";
        }
    }
    for (const auto &item : s.nested_table) {
        os << "  " << item.first << "\n";
        for (const auto &subitem : item.second) {
            os << "    " << subitem.first << ": " << subitem.second << "\n";
        }
    }
    return os;
}

void to_json(nlohmann::basic_json<> &j, const UserConfig &s) {
    j = nlohmann::basic_json<>{
        {"top-string", s.top_string},
        {"top-int", s.top_int},
        {"top-bool", s.top_bool},
        {"top-list", s.top_array},
        {"top-dict", s.top_table}};
}

void from_json(const nlohmann::basic_json<> &j, UserConfig &s) {
    j.at("top-string").get_to(s.top_string);
    j.at("top-int").get_to(s.top_int);
    j.at("top-bool").get_to(s.top_bool);
    j.at("top-list").get_to(s.top_array);
    j.at("top-dict").get_to(s.top_table);
    j.at("nested-list").get_to(s.nested_array);
    j.at("nested-dict").get_to(s.nested_table);
}
