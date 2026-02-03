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
    double pi{3.1415};
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

    static Config& instance() {
        static Config instance;
        return instance;
    }

    const ConfigData& data() const { return data_; }
    ConfigData& data() { return data_; }
    CLI::App& cli() { return cli_; }
    toml::value& toml_root() { return toml_root_; }
    const std::string& config_file() const { return config_file_; }

    bool write_config() const {
        if (!file_exists(config_file_)) {
            std::cerr << "Failed to open file: " << config_file_ << std::endl;
            return false;
        }

        return write_to_file(config_file_);
    }

    bool safe_write_config() const {
        if (file_exists(config_file_)) {
            std::cerr << "File exists: " << config_file_ << std::endl;
            return false;
        }
        return write_to_file(config_file_);
    }

    bool write_config_as(const std::string& filename) const {
        return write_to_file(filename);
    }

    bool safe_write_config_as(const std::string& filename) const {
        if (file_exists(filename)) {
            std::cerr << "File exists: " << filename << std::endl;
            return false;
        }
        return write_to_file(filename);
    }

    void parse(int argc, char** argv) {
        try {
            // parse command line and config file related to cli
            cli_.parse(argc, argv);
            config_file_ = cli_.get_config_ptr()->as<std::string>();
            std::cout << "Loaded configuration from: " << cli_.get_config_ptr()->as<std::string>() << std::endl;
            toml_root_ = toml::parse(config_file_);

            override_toml();
        } catch (const CLI::ParseError& e) {
            std::exit(cli_.exit(e));
        } catch (const toml::exception& e) {
            std::cerr << e.what() << std::endl;
            std::exit(1);
        } catch (const std::exception& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            std::exit(1);
        }
    }

    std::string cli_to_string(bool default_also = false, bool write_description = false) { return cli_.config_to_str(default_also, write_description); }

    std::string toml_to_string() { return toml::format(toml_root_); }

    std::string data_to_string() {
        std::stringstream ss;
        ss << "data_.dae    : " << data_.dae << std::endl;
        ss << "data_.port   : " << data_.port << std::endl;
        ss << "data_.env    : " << data_.env << std::endl;
        ss << "data_.pi     : " << data_.pi << std::endl;
        ss << "data_.sub.sub: " << data_.sub.sub << std::endl;
        return ss.str();
    }

  private:
    explicit Config() : cli_("Lynx Network Daemon") {
        cli_.add_flag("-d,--dae", data_.dae, "Running program in daemon mode")
            ->group("Important");

        cli_.add_option("--pi", data_.pi, "Set pi value");

        cli_.add_option("-p,--port", data_.port, "Program telnet port")
            ->group("Important")
            ->check(CLI::Range(1024, 65535));

        // config file
        cli_.set_config("--config", "lynx.toml", "Read an toml file", true)
            ->transform(CLI::FileOnDefaultPath("/etc"))
            ->transform(CLI::FileOnDefaultPath("./etc", false))
            ->transform(CLI::FileOnDefaultPath("./", false));  // first match

        // environment
        cli_.add_option("--env", data_.env)->envname("MY_ENV");
        // cli.add_option("--myenv", opt)->envname("MY_ENV")->check(CLI::ValidIPV4);

        // subcommand
        CLI::App* sub = cli_.add_subcommand("sub", "This is a subcommand");
        sub->add_flag("-s,--sub", data_.sub.sub, "dae in sub");
    }

    void override_toml() {
        // toml_root_["pi"].as_floating_fmt().prec = 16;
        toml_root_["pi"] = data_.pi;
        toml_root_["port"] = data_.port;
        toml_root_["env"] = data_.env;
        toml_root_["dae"] = data_.dae;
        toml_root_["sub"]["sub"] = data_.sub.sub;
    }

    bool file_exists(const std::string& filename) const {
        std::ifstream file(filename);
        return file.good();
    }

    bool write_to_file(const std::string& filename) const {
        std::ofstream out_file(filename);
        if (!out_file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }

        out_file << toml::format(toml_root_);
        out_file.close();
        std::cout << "Successfully saved config to: " << filename << std::endl;
        return true;
    }

    // 配置数据存储
    ConfigData data_;
    CLI::App cli_;
    toml::value toml_root_;
    std::string config_file_;
};

}  // namespace lynx
