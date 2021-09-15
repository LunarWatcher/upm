#pragma once

#include "nlohmann/json.hpp"
#include "stc/Environment.hpp"

namespace upm {

class Config {
public:
    const nlohmann::json data;
    Config();
    
};

}
