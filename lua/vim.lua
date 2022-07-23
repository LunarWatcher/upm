local network = require "upmnetwork"
local exec = require "upmexec"
local fs = require "upmfs"

-- There may be more dependencies than this; my own config seems to be a bit excessive, will have to test
-- later. Or wait for someone else to use upm and report missing dependencies.
fs.sharedLibInstalled("libXt.so", "libgtk-3.so", 
    "libncurses.so", -- seems to use libncurses.so.<version> with no base or dashes elsewhere. Arsed defining which,
                     -- so it's easier to just define .so and hope the pattern remains constant
    "libpango-1.0.so", "libcairo.so", "libatk-1.0.so");
-- Okay, but what the fuck do I do about Lua? And Python? They're more annoying to do in general
-- because versions. 5.1 is mainstream because luajit, but 5.3 has worked for me so far. Unless
-- that's 5.1 kicking in to override? I don't fucking know.

-- TODO: figure out how the fuck we're doing context sharing.
-- We need to allow lua scripts to sort out their own version formats, so we kind of have 
-- to pass it to Lua.
-- I don't know, maybe a new API?

-- Note: we _have_ to clean Vim with a nuke. I'm sure there's commands that work,
-- but this worked in my makefile, and makes sure shit works if shit goes sideways.
-- maybe `make distclean` is what I'm looking for as an alternative?
-- We already have a return value to check if it was re-cloned or not,
-- and clone failure throws an error for the user to handle.
network.gitClone("https://github.com/vim/vim", "vim", true);
