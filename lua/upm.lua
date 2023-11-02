local fs = require "upmfs"
local vResolvers = require "vResolvers"
local git = require "git";
local activators = require "activators"
local log = require "upmlog"

local function install()
    local installSelf = ctx["options"]["install_self"] ~= nil;
    local directory = nil;
    if (installSelf) then
        log.info("Note: Using cwd as build directory");
        directory = fs.pwd();
    else
        log.debug("Cloning from GitHub");
        _, directory = git.clone(
            "https://github.com/LunarWatcher/upm",
            "upm"
        );
    end

    vResolvers.git(directory, true);
    ctx:checkInstalled();

    fs.cmake(directory, {
        cd = not installSelf
    });

end

local function apply()
    local upm = ctx["prefix"] .. "/bin/upm";
    activators.activateSingle(upm, "bin/upm");
    activators.activateSingle(ctx["prefix"] .. "/lua", "/opt/upm/lua")
end

return {
    install = install,
    apply = apply
}
