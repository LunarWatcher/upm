local git = require "git"
local network = require "upmnetwork"
local fs = require "upmfs"
local log = require "upmlog"
local activators = require "activators"
local vResolvers = require "vResolvers"

function install()

    -- very preliminary code

    -- The easy option here is to use the git repo to get the latest version.
    -- This also makes it easier to sort out compilation in the future.
    local _, gitDir = git.clone("https://github.com/baldurk/renderdoc", "renderdoc")
    -- Resolve the version. Nightly will fail for now, because it assumes
    -- a manual build.
    local version = vResolvers.git(gitDir, true)

    local os, cpuArch = ctx:getArch()
    if os == "linux" then
        -- I haven't verified, but I assume "64-bit Linux" means x86_64
        if cpuArch ~= "x64" then
            error("Not yet supported");
        end

        local stripV = version
        if (stripV:sub(1, 1) == 'v') then
            stripV = stripV:sub(2)
        end
        local url = "https://renderdoc.org/stable/" .. tostring(stripV)
            .. "/renderdoc_" .. tostring(stripV) .. ".tar.gz";
        log.info("Downloading renderdoc from", url)
        local _, filePath = network.download(url);
        if (filePath ~= nil) then
            local dest = fs.untar(filePath, 1)
            fs.installCopy(dest)
        else
            error("Failed to download binary tarball.")
        end
    else
        error("Not yet supported")
    end

end

return {
    install = install,
    apply = activators.universalUNIX
}
