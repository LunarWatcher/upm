local network = require "upmnetwork"
local exec = require "upmexec"
local fs = require "upmfs"

exec.exec("echo \"hi\"");

fs.sharedLibInstalled("libXt.so", "libgtk-3.so");
