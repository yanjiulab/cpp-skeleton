#pragma once

#include <cstdio>
#include <iostream>
#include <string>

#include "CLI11.hpp"
#include "config.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/spdlog.h"
#include "toml.hpp"

#define OVERRIDE_TOML_VAR(var) root[#var] = data.var;
#define OVERRIDE_TOML_VARS(...) TOML11_FOR_EACH_VA_ARGS(OVERRIDE_TOML_VAR, __VA_ARGS__)

// 首先 config.toml 会决定 cmd 参数的值，如果提供了文件，并将结果反应至 data 中，或者使用 app 访问同理。
// 其次使用 toml::parse 解析 config.toml，注意，此时不会覆盖 cmd 中的值，如果先用 toml parse 了，后续会更改 data
// 但是使用 toml::find 仍然是配置文件中的参数，导致不一致，因此，直接使用 data，不要再进行解析。
// 直接将 data 进行一次序列化，生成新的 toml 结构体，并将其写入配置文件。
// 进行触发式读取，获取 toml。

// Parse order:
// args => CLI::App cli and UserConfigData data;
// file(related to cli) => CLI::App cli and UserConfigData data;
// env  => CLI::App cli and UserConfigData data;
// file(others) => toml::root
// using default
struct UserConfigData {
    // cli data
    bool dae{false};
    int port{0};
    std::string env;
    double pi{3.14};
};
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UserConfigData, dae, port, env, pi)
// TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(UserConfigData, dae, port, env, pi)

class UserConfig {
  public:
    // top data
    CLI::App cli;
    toml::value root;
    // store data
    UserConfigData data;

    explicit UserConfig(int argc, char** argv) {
        cli.add_flag_function(
            "-v,--version", [](int count) {
        if (count >= 1) {
            printf("version: %d.%d.%d\n", MYPROJECT_VERSION_MAJOR,
                                        MYPROJECT_VERSION_MINOR,
                                        MYPROJECT_VERSION_PATCH);
        }
        if (count >= 2) {
            const std::string v2 = fmt::format("{} {}", __DATE__, __TIME__);
            std::cout << "Compile time: " << v2 << std::endl;
        }
        
        if (count >= 3) {
            const std::string v3;
            std::cout << "Program MD5: " << v3 << std::endl;
            printf("asio version: %d.%d.%d\n", ASIO_VERSION / 100000, ASIO_VERSION / 100 % 1000, 
            ASIO_VERSION % 100);
            printf("spdlog version: %d.%d.%d\n", SPDLOG_VER_MAJOR, SPDLOG_VER_MINOR, SPDLOG_VER_PATCH);
            printf("CLI11 version: %s\n", CLI11_VERSION);
            printf("json version: %d.%d.%d\n", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH);              
        }
        std::exit(0); }, "Display program version information and exit");

        cli.add_flag("-d,--dae", data.dae, "Running program in daemon mode")
            ->group("Important");

        cli.add_option("--pi", data.pi, "Set pi value");

        cli.add_option("-p,--port", data.port, "Program telnet port")
            ->group("Important")
            ->check(CLI::Range(1024, 65535));

        // config file
        cli.set_config("--config", "config.toml", "Read an toml file", false)
            ->transform(CLI::FileOnDefaultPath("./etc"))
            ->transform(CLI::FileOnDefaultPath("./", false));  // first match
        config_file_ = cli.get_config_ptr()->as<std::string>();

        // environment
        cli.add_option("--env", data.env)->envname("MY_ENV");
        // cli.add_option("--myenv", opt)->envname("MY_ENV")->check(CLI::ValidIPV4);

        // subcommand
        CLI::App* sub = cli.add_subcommand("sub", "This is a subcommand");

        try {
            // parse command line and config file related to cli
            cli.parse(argc, argv);

            // parse config file
            root = toml::parse(config_file_);
            std::cout << "Load config file: " << config_file_ << std::endl;
            OVERRIDE_TOML_VARS(pi, port, env, dae);  // overrided via cli parse
            std::cout << "[START] Configuration =========" << std::endl;
            std::cout << "[START] cmd args --------------" << std::endl;
            // std::cout << cli.config_to_str(true, true) << std::endl;
            std::cout << cli.config_to_str(true, false) << std::endl;
            std::cout << "[START] file root -------------" << std::endl;
            // std::cout << cli.config_to_str(true, true) << std::endl;
            std::cout << toml::format(root) << std::endl;
            std::cout << "[END] file root ---------------" << std::endl;
            std::cout << "[END] Configuration ===========" << std::endl;
        } catch (const CLI::ParseError& e) {
            std::exit(cli.exit(e));
        }
    }

    int WriteConfig(bool overrided) {
        std::string name;
        if (overrided) {
            name = config_file_;
        } else {
            name = config_file_ + ".sav";
        }

        std::ofstream out_file(name);
        if (!out_file.is_open()) {
            std::cerr << "Failed to open file: " << config_file_ << std::endl;
            return -1;
        }

        out_file << toml::format(root);
        out_file.close();
        std::cout << "TOML file written successfully!" << std::endl;
        return 0;
    }

    std::string config_file() const { return config_file_; }

  private:
    std::string config_file_;
};

// }  // namespace daemon

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
