local network = require "upmnetwork"
local exec = require "upmexec"
local fs = require "upmfs"
local vResolvers = require "vResolvers"
local log = require "upmlog"
local git = require "git";

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

end

return {
    install = install,
    apply = apply
}
