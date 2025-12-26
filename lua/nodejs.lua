local network = require "upmnetwork"
local fs = require "upmfs"
local json = require "json"
local activators = require "activators"
local log = require "upmlog"

local function install()
    ---@type string
    local version = ctx["version"];

    if (version == "latest" or version == "lts") then
        log.info("Attempting to resolve version...");
        local r = network.request("https://nodejs.org/dist/index.json");
        if (r.status_code ~= 200) then
            error("Failed to request node version index: " .. r.error_message);
            -- Probably redundant
            return
        end
        local obj = json.parse(r["text"]);
        if (version == "latest") then
            -- the element at index 0 is always the latest release.
            -- But of course, because lua is 1-indexed, that means index
            -- 1 in its native array.
            version = obj[1]["version"]
        else
            -- TODO: #obj?
            for i = 1, string.len(obj) do
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
        if (tonumber(version, 10) ~= nil) then
            -- The passed version is exclusively a number. We assume @xx, and convert to latest-vxx
            -- TODO: is format() a thing?
            version = "latest-v" .. version .. ".x"
        else
            version = "v" .. version
        end
        local r = network.request("https://nodejs.org/dist/" .. version)
        if (r["status_code"] ~= 200) then
            error("Invalid version supplied; nodejs.org/dist/" .. version .. " couldn't be resolved.")
        end
    end
    log.info("Resolved version:", version);
    ctx["resolvedVersion"] = version
    ctx:checkInstalled()

    local os, arch = ctx:getArch();

    log.info("Resolved OS, arch:", os, arch)
    if (os ~= "UNK" and arch ~= "UNK") then
        local _, filePath = network.download("https://nodejs.org/dist/" .. version .. "/node-" .. version .. "-" .. os .. "-" .. arch .. ".tar.gz")
        if (filePath ~= nil) then
            local dest = fs.untar(filePath, 1)
            fs.installCopy(dest)
            return
        end
    end
    error("Compiling from source is not supported")
end

local function apply()
    activators.universalUNIX({ "node", "node_modules" })
end

return {
    install = install,
    apply = apply
}
