local fs = require "upmfs"
local vResolvers = require "vResolvers"
local git = require "git";
local activators = require "activators"

local function install()
    local _, directory = git.clone(
        "https://github.com/LunarWatcher/upm",
        "upm",
        false,
        true
    );


    vResolvers.git(directory, true);
    ctx:checkInstalled();

    fs.cmake(directory);

end

local function apply()
    local upm = ctx["prefix"] .. "/bin/upm";
    activators.activateSingle(upm, "bin/upm");
end

return {
    install = install,
    apply = apply
}
