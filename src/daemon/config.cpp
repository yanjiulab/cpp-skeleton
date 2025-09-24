#include "config.hpp"

#include <iostream>

#include "toml.hpp"

// int parse() {
//     // select TOML version at runtime (optional)
//     auto data = toml::parse("etc/example.toml");

//     // find a value with the specified type from a table
//     std::string title = toml::find<std::string>(data, "title");

//     // convert the whole array into STL-like container automatically
//     std::vector<int> nums = toml::find<std::vector<int>>(data, "nums");

//     // access with STL-like manner
//     if (!data.contains("foo")) {
//         data["foo"] = "bar";
//     }
//     if (data.at("nums").is_array()) {
//         data["nums"].as_array().push_back(9);
//     }

//     // check comments
//     assert(data.at("nums").comments().at(0) == "# pi!");

//     // pass a fallback
//     std::string name = toml::find_or<std::string>(data, "name", "not found");

//     // serialization considering format info
//     data.at("nums").as_array_fmt().fmt = toml::array_format::multiline;
//     data.at("nums").as_array_fmt().indent_type = toml::indent_char::space;
//     data.at("nums").as_array_fmt().body_indent = 2;

//     std::cout << toml::format(data) << std::endl;

//     return 0;
// }
