local network = require "upmnetwork"
local exec = require "upmexec"
local fs = require "upmfs"
local context = require "upmcontext"
local json = require "json"

function install() {
    version = context["version"];
        
    if (version == "latest" or version == "lts") {
        r = network.request("https://nodejs.org/dist/index.json");
        if (r.status_code != 200) {
            error("Failed to request node version index");
            -- Probably redundant
            return
        }
        obj = json.parse(r["text"]);


    }
    
}
