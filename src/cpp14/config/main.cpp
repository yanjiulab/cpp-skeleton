// Sample application using ConfigCpp to handle json-based configuration file

#include <iostream>

#include "config-cpp/config-cpp.h"

// User-provided type corresponding to json configuration data
#include "user-config.h"

void onConfigChange(ConfigCpp::ConfigCpp &config) {
    config.ReadInConfig();
    std::cout << "in onConfigChange().  Updated config is:\n"
              << config.GetConfigData() << "\n";
}

int main(int argc, char **argv) {
    ConfigCpp::ConfigCpp config(argc, argv);

    config.SetConfigName("config");
    config.SetConfigType(ConfigCpp::ConfigType::TOML);

    // config.AddConfigPath("../inputs/json/");
    // config.AddConfigPath("../../inputs/json/");
    config.AddConfigPath("./etc/");

    config.AddBoolOption("b,top-bool", "Bool option");
    config.SetDefault("top-bool", false);
    config.AddIntOption("t,top-int", "Integer option");
    config.SetDefault("top-int", 47);
    config.AddStringOption("s,top-string", "String option");
    config.AddDoubleOption("d,top-double", "Double option");

    if (config.ReadInConfig()) {
        std::cout << "Read in config successfully:\n"
                  << config.GetConfigData() << "\n";

        // Unmarshal all config data to user-provided type
        UserConfig configData;
        if (config.GetConfigType() == ConfigCpp::ConfigType::JSON) {
            if (config.UnmarshalJson<UserConfig>(configData)) {
                std::cout << "Unmarshalled config successfully:\n"
                          << configData;
            } else {
                std::cout << "Failed to ummarshal config\n";
            }
        } else if (config.GetConfigType() == ConfigCpp::ConfigType::TOML) {
            if (config.UnmarshalToml<UserConfig>(configData)) {
                std::cout << "Unmarshalled config successfully:\n"
                          << configData;
            } else {
                std::cout << "Failed to ummarshal config\n";
            }
        } else {
            std::cout << "Unknown config type\n";
        }

        // Retrieve specific config values read from yaml:
        auto stringVal = config.GetString("top-string");
        std::cout << "StringVal: " << stringVal << "\n";
        auto intVal = config.GetInt("top-int");
        std::cout << "IntVal: " << intVal << "\n";
        auto dictVal = config.GetString("nested-table.key2.key2-subkey2");
        std::cout << "DictVal: " << dictVal << "\n";

        config.SetBool("top-bool", false);
        auto newVal = config.GetBool("top-bool");
        std::cout << "newVal: " << newVal << "\n";

        auto val = config.GetBool("nested-table.key1.key1-subkey1");
        std::cout << "OldDictBool: " << val << "\n";
        config.SetBool("nested-table.key1.key1-subkey1", false);
        auto val1 = config.GetBool("nested-table.key1.key1-subkey1");
        std::cout << "NewDictBool: " << val1 << "\n";

        config.SetInt("fuck.you.off", 999);
        config.SetDouble("heal.the", 3.2231);
        config.SetString("all", "shit");
        config.SetString("fuck.off", "too shit");

        std::cout << config.GetConfigData() << "\n";

        config.WriteConfig();

        config.OnConfigChange(onConfigChange);
        config.WatchConfig();
        std::cout << "Waiting for config changes\n";

        while (true) {
            sleep(1);
        }

    } else {
        std::cout << "Failed to read in config\n";
    }
}