local network = require "upmnetwork"
local exec = require "upmexec"
local fs = require "upmfs"

print(ctx)
print(type(ctx))
print("Package", ctx.package);
print("Version", ctx.version);
print("Dummy test call to validate metatable function calls", ctx:test());

-- There may be more dependencies than this; my own config seems to be a bit excessive, will have to test
-- later. Or wait for someone else to use upm and report missing dependencies.
fs.sharedLibInstalled(true, "libXt.so", "libgtk-3.so",
    "libncurses.so", -- seems to use libncurses.so.<version> with no base or dashes elsewhere. Arsed defining which,
                     -- so it's easier to just define .so and hope the pattern remains constant
    "libpango-1.0.so", "libcairo.so", "libatk-1.0.so");

-- TODO: revisit; not convinced this is the best approach
local hasLua = fs.sharedLibInstalled(false, "liblua5");
local hasRuby = fs.sharedLibInstalled(false, "libruby-");
local hasPython3 = fs.sharedLibInstalled(false, "libpython3");
local hasPerl = fs.sharedLibInstalled(false, "libperl.so");

-- TODO: find a better way to log config.
print("Lua:", hasLua);
print("Ruby:", hasRuby);
print("Python 3:", hasPython3);
print("Perl:", hasPerl);


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
--local _, directory = network.gitClone("https://github.com/vim/vim", "vim", true);

--local extras = ""

--if hasLua then
    --extras += "--enable-luainterp "
--end
--if hasRuby then
    --extras += "--enable-rubyinterp "
--end
--if hasPython3 then
    --extras += "--enable-python3interp "
--end
--if hasPerl then
    --extras += "--enable-perlinterp "
--end

---- TODO: make configure an API function, because this is nasty as fuck.
---- Also no prefix here, which is something upm should deal with, which means we're right back
---- to delivering Context to the scripts somehow.
---- I still feel making Context a static class and accessing it through the endpoints makes more
---- sense. Making a full Lua object for it is a pain in the ass.
---- On the other hand, that _is_ how a good Lua API would do it.
---- But now we're over on metatables, and I haven't looked too much into that...
---- As long as it's a proxy somehow, I _guess_ it makes sense, but I really want to avoid excess
---- data duplication and for the sake of preseving sync,
---- but i'm not convinced there's enough bidirectional data for it to be worth it.
---- Have fun with this, tomorrow me
----
---- ... me a couple minute slater here, what about InstallationResolver::install? It admittedly does
---- download and doesn't interface with git, but it does have the start for the source builds.
---- Unpacking remains a problem as well, just not for this particular script.
---- I'll have to revisit quite a chunk of this tomorrow.
--exec.exec("cd " .. directory .. "/src &&"
    --.. " ./configure --enable-gui=gtk3 --with-features=huge --enable-multibyte "
    --.. extras
    --.. "--prefix="
    --.. "--enable-fail-if-missing --with-compiledby=\"Olivia/upm\""
--)
