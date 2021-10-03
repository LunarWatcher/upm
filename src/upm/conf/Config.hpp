#pragma once

#include "nlohmann/json.hpp"
#include "stc/Environment.hpp"

namespace upm {

class Context;
class Config {
private:
    Context* ctx;
    fs::path confPath;
    
public:
    nlohmann::json data;

    Config(Context* ctx);
    ~Config();
};

}
