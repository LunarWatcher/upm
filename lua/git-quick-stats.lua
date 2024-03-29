local git = require "git"
local fs = require "upmfs"
local log = require "upmlog"
local activators = require "activators"
local vResolvers = require "vResolvers"

function install()

    local _, directory = git.clone("https://github.com/arzzen/git-quick-stats", "git-quick-stats", false);
    version = vResolvers.git(directory)
    ctx:checkInstalled()

    log.info("Running make...")
    fs.makeInstallOnly(directory, "", "make PREFIX=" .. ctx["prefix"], -1)
    log.info("Done.")
end

return {
    install = install,
    apply = activators.universalUNIX
}
