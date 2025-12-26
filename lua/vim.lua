local git = require "git"
local exec = require "upmexec"
local fs = require "upmfs"
local log = require "upmlog"
local activators = require "activators"
local vResolvers = require "vResolvers"

function install()

    log.info("Checking for required dependencies....");
    -- There may be more dependencies than this; my own config seems to be a bit excessive, will have to test
    -- later. Or wait for someone else to use upm and report missing dependencies.
    fs.sharedLibInstalled(true, "libXt.so", "libgtk-3.so",
        "libncurses.so", -- seems to use libncurses.so.<version> with no base or dashes elsewhere. Arsed defining which,
                         -- so it's easier to just define .so and hope the pattern remains constant
        "libpango-1.0.so", "libcairo.so", "libatk-1.0.so");

    log.info("Checking for optional feature support...");
    -- TODO: revisit; not convinced this is the best approach
    local hasLua = fs.sharedLibInstalled(false, "liblua5");
    local hasRuby = fs.sharedLibInstalled(false, "libruby-");
    local hasPython3 = fs.sharedLibInstalled(false, "libpython3");
    local hasPerl = fs.sharedLibInstalled(false, "libperl.so");

    log.info("Compiling with:")
    log.info("Lua:", hasLua);
    log.info("Ruby:", hasRuby);
    log.info("Python 3:", hasPython3);
    log.info("Perl:", hasPerl);
    log.warn("Note: upm is currently unable to verify if you have development headers. If you're missing features, "
        .. "you'll likely need more dev packages"
    );
    print()

    -- Note: we _have_ to clean Vim with a nuke. I'm sure there's commands that work,
    -- but this worked in my makefile, and makes sure shit works if shit goes sideways.
    -- maybe `make distclean` is what I'm looking for as an alternative?
    -- We already have a return value to check if it was re-cloned or not,
    -- and clone failure throws an error for the user to handle.
    --- @type string
    local _, directory = git.clone(
        "https://github.com/vim/vim",
        "vim",
        true
    );
    vResolvers.git(directory, true)
    ctx:checkInstalled()

    local args = {
        "--enable-gui=gtk3",
        "--with-features=huge",
        "--enable-multibyte",
        "--with-compiledby=Olivia (LunarWatcher/upm)"
    }

    if hasLua then
        table.insert(args, "--enable-luainterp")
    end
    if hasRuby then
        table.insert(args, "--enable-rubyinterp")
    end
    if hasPython3 then
        table.insert(args, "--enable-python3interp")
    end
    if hasPerl then
        table.insert(args, "--enable-perlinterp")
    end

    log.info("Preparing configure...");
    fs.configure(directory .. "/src", args)

    log.info("Preparing install...");
    fs.make(directory .. "/src", {}, -1)
    log.info("Done.");
end

return {
    install = install,
    apply = activators.universalUNIX
}
