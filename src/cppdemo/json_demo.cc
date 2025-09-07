#include <iostream>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

int json_demo() {
    json j;
    j["name"] = "Alice";
    j["age"] = 25;
    j["hobbies"] = {"reading", "hiking"};
    j["is_student"] = false;

    std::string jstr = j.dump(4);

    json j2 = json::parse(jstr);

    std::cout << jstr << std::endl;
    std::cout << j2 << std::endl;

    json j3 = {"key", 1};
    std::cout << j3 << std::endl;

    return 0;
}