#pragma once

#include "nlohmann/json.hpp"
#include <filesystem>

namespace upm {

class Context;
class Config {
private:
    Context* ctx;
    std::filesystem::path confPath;
    
    void setup();
public:
    nlohmann::json data;

    Config(Context* ctx);
    ~Config();

    /**
     * Forces the config to save. Particularly useful to avoid data loss from a crash
     *
     * Note that this function is automatically called when the config instance is destroyed, but explicit saving
     * is generally a good idea.
     */
    void save();

    nlohmann::json& operator[](const std::string& key) {
        return data[key];
    }
};

}
