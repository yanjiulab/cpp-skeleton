#pragma once

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "CLI11.hpp"
#include "config.h"
#include "toml.hpp"


namespace lynx {
struct ConfigData {
    // cli data
    bool dae{false};
    int port{0};
    std::string env;
    double pi{3.14};
    struct Sub {
        bool sub{false};
    } sub;
};
}  // namespace lynx
TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(lynx::ConfigData::Sub, sub)
TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(lynx::ConfigData, dae, port, env, pi, sub)

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

// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigData, dae, port, env, pi)
namespace lynx {
class Config {
  public:
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    static Config& Instance() {
        static Config instance;
        return instance;
    }

    const ConfigData& data() const { return data_; }
    ConfigData& data() { return data_; }

    std::string config_file() const { return config_file_; }

    int SaveToFile(bool overrided = false) const {
        std::string file_name = overrided ? config_file_ : config_file_ + ".sav";

        std::ofstream out_file(file_name);
        if (!out_file.is_open()) {
            std::cerr << "Failed to open file: " << config_file_ << std::endl;
            return -1;
        }

        out_file << toml::format(toml_root_);
        out_file.close();
        std::cout << "Successfully saved config to: " << file_name << std::endl;
        return 0;
    }

    void parse(int argc, char** argv) {
        try {
            // parse command line and config file related to cli
            cli_.parse(argc, argv);
            std::cout << "Loaded configuration from: " << config_file_ << std::endl;

            toml_root_ = toml::parse(config_file_);

            OverrideToml();

            PrintConfigSummary();
        } catch (const CLI::ParseError& e) {
            std::cerr << "CLI parse error: " << e.what() << std::endl;
            std::exit(cli_.exit(e));
        } catch (const toml::exception& e) {
            std::cerr << "Toml parse error: " << e.what() << std::endl;
            std::exit(1);
        } catch (const std::exception& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            std::exit(1);
        }
    }

    void PrintConfigSummary() const {
        std::cout << "======================================" << std::endl;
        std::cout << "Configuration Summary" << std::endl;
        std::cout << "--------------------------------------" << std::endl;
        // std::cout << cli.config_to_str(true, true) << std::endl;
        std::cout << cli_.config_to_str(true, false) << std::endl;
        std::cout << "--------------------------------------" << std::endl;
        std::cout << toml::format(toml_root_) << std::endl;
        std::cout << "--------------------------------------" << std::endl;
        PrintData();
        std::cout << "======================================" << std::endl;
    }

    void PrintData() const {
        std::cout << "data_.dae    : " << data_.dae << std::endl;
        std::cout << "data_.port   : " << data_.port << std::endl;
        std::cout << "data_.env    : " << data_.env << std::endl;
        std::cout << "data_.pi     : " << data_.pi << std::endl;
        std::cout << "data_.sub.sub: " << data_.sub.sub << std::endl;
    }

    CLI::App& cli() { return cli_; }
    toml::value& toml_root() { return toml_root_; }

  private:
    explicit Config() : cli_("Lynx Network Daemon") {
        cli_.add_flag("-d,--dae", data_.dae, "Running program in daemon mode")
            ->group("Important");

        cli_.add_option("--pi", data_.pi, "Set pi value");

        cli_.add_option("-p,--port", data_.port, "Program telnet port")
            ->group("Important")
            ->check(CLI::Range(1024, 65535));

        // config file
        cli_.set_config("--config", "config.toml", "Read an toml file", false)
            ->transform(CLI::FileOnDefaultPath("./etc"))
            ->transform(CLI::FileOnDefaultPath("./", false));  // first match
        config_file_ = cli_.get_config_ptr()->as<std::string>();

        // environment
        cli_.add_option("--env", data_.env)->envname("MY_ENV");
        // cli.add_option("--myenv", opt)->envname("MY_ENV")->check(CLI::ValidIPV4);

        // subcommand
        CLI::App* sub = cli_.add_subcommand("sub", "This is a subcommand");
        sub->add_flag("-s,--sub", data_.sub.sub, "dae in sub");
    }

    void OverrideToml() {
        toml_root_["pi"] = data_.pi;
        toml_root_["port"] = data_.port;
        toml_root_["env"] = data_.env;
        toml_root_["dae"] = data_.dae;
        toml_root_["sub"]["sub"] = data_.sub.sub;
    }

    // 打印配置摘要

    // 配置数据存储
    ConfigData data_;
    CLI::App cli_;
    toml::value toml_root_;
    std::string config_file_;
};

}  // namespace lynx
