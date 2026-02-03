#include "rest_server.hpp"

namespace lynx {
void RestServer::setup_routes() {
    // REST API 示例1：数字参数
    server_.set_http_handler<cinatra::GET, cinatra::POST>(
        R"(/numbers/(\d+)/test/(\d+))",
        [](cinatra::request& req, cinatra::response& res) {
            std::cout << "matches[1] is : " << req.matches_[1]
                      << " matches[2] is: " << req.matches_[2] << std::endl;
            res.set_status_and_content(cinatra::status_type::ok, "hello world");
        });

    // REST API 示例2：字符串参数
    server_.set_http_handler<cinatra::GET, cinatra::POST>(
        "/string/:id/test/:name",
        [](cinatra::request& req, cinatra::response& res) {
            std::string id = req.params_["id"];
            std::string name = req.params_["name"];
            std::cout << "id value is: " << id << std::endl;
            std::cout << "name value is: " << name << std::endl;
            res.set_status_and_content(cinatra::status_type::ok, name);
        });
}
}  // namespace lynx