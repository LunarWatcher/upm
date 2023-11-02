#pragma once

#include <lua.hpp>
#include <map>
extern "C" {
#include "lualib.h"
}

#include "Network.hpp"

namespace upm {

class LuaHelper {
private:
    lua_State* state;

    std::map<std::string, int> luaCache;
public:
    LuaHelper();
    ~LuaHelper();

    void init();

    /**
     * Runs a file at a provided path.
     *
     * Note that the filename has to be the full (relative or absolute) path to the lua file.
     * No changes are made to the filename to fit various upm conventions, as other parts of the code deal
     * with this.
     */
    void runFile(const std::string& fn);

    /**
     * Identical to runFile, but with additional validation for the return results.
     *
     * Note that this method throws, and does not clean up the stack first. The programmer is responsible for clearing the stack
     * of anything that shouldn't be there.
     */
    void runFileForResult(const std::string& fn, int nRes, std::vector<int> types);
    void runString(const std::string& script);

    void dump();
    static void dump(lua_State* state);
    
    void install();

    // Why the fuck do I need this??
    void registerLibrary(const char* libname, lua_CFunction func);
    decltype(state) getState() { return state; }

    decltype(state) operator*() { return state; }
};

}
