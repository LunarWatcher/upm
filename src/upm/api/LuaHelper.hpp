#pragma once

#include <lua.hpp>
extern "C" {
#include "lualib.h"
}

#include "Network.hpp"

namespace upm {

class LuaHelper {
private:
    lua_State* state;
public:
    LuaHelper();
    ~LuaHelper();

    void init();
    void runFile(const std::string& fn);
    void runString(const std::string& script);

    void dump();
    static void dump(lua_State* state);
    
    void install();

    // Why the fuck do I need this??
    void registerLibrary(const char* libname, lua_CFunction func);
    decltype(state) getState() { return state; }
};

}
