local network = require "upmnetwork"
local exec = require "upmexec"
local fs = require "upmfs"
local json = require "json"
local activators = require "activators"
local log = require "upmlog"

function install()
    version = ctx["version"];

    if (version == "latest" or version == "lts") then
        log.info("Attempting to resolve version...");
        r = network.request("https://nodejs.org/dist/index.json");
        if (r.status_code ~= 200) then
            error("Failed to request node version index");
            -- Probably redundant
            return
        end
        obj = json.parse(r["text"]);
        if (version == "latest") then
            -- the element at index 0 is always the latest release.
            -- But of course, because lua is 1-indexed, that means index
            -- 1 in its native array.
            version = obj[1]["version"]
        else
            for i = 1, len(obj) do
                if (obj[i]["lts"] == true) then
                    version = obj[i]["version"]
                    break;
                end
            end
        end

        if (version:sub(1, 1) ~= 'v') then
            error("Failed to resolve version: found " .. version);
        end
    elseif (version:sub(1, 1) ~= 'v') then
        version = "v" .. version
        r = network.request("https://nodejs.org/dist/" .. version)
        if (r["status_code"] ~= 200) then
            error("Invalid version supplied; nodejs.org/dist/" .. version .. " couldn't be resolved.")
        end
    end
    log.info("Resolved version:", version);
    ctx["resolvedVersion"] = version
    ctx:checkInstalled()

    os, arch = ctx:getArch();

    log.info("Resolved OS, arch:", os, arch)
    if (os ~= "UNK" and arch ~= "UNK") then
        status, filePath = network.download("https://nodejs.org/dist/" .. version .. "/node-" .. version .. "-" .. os .. "-" .. arch .. ".tar.gz")
        if (filePath ~= nil) then
            dest = fs.untar(filePath, 1)
            fs.installCopy(dest)
            return
        end
        
    end
    error("Compiling from source is not supported")
end

return {
    install = install,
    apply = activators.universalUNIX
}
