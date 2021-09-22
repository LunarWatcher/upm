#pragma once

#include "nlohmann/json.hpp"
#include "stc/Environment.hpp"

namespace upm {

class Context;
class Config {
private:
    Context* ctx;
    
public:
    nlohmann::json data;

    Config(Context* ctx);
};

}
