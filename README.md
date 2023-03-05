# Upstream Package Manager (upm)

Package manager manager and SDK manager, because someone had to make the 15th standard.

[![xkcd: standards][1]][1]

---

## What?

Installs shit so you don't have to. This is, in essence, a gigantic automation project (and an excuse for me to get acquainted with the Lua C API, but that's a different story).

## Like?

Whatever has been set up.

## How?

Through a combination of GitHub repositories, automatic redirection, and good 'ol fashioned manual link following. And of course lists for when absolutely everything else fails.

## Requirements?

A C++17 compiler, and Linux or MacOS, and access to `tar` with a few specs (see the tar section). Note that MacOS will have weaker support, because I don't have access to any Mac machines. Please consider contributing to the project if you have access to a Mac.

General UNIXes beyond Linux and MacOS will have varying support, for the same reason MacOS doesn't have full support: I don't have those operating systems, and testing them is a slow and annoying process that I don't want to spend my time on.


### Tar (temporary; see #20)

* Supports `--strip-components`
* Supports automatic tar type detection*

\*: may be dropped if it turns out there's a lot of tars that support `--strip-components`, but somehow not automatic type detection. Feedback needed.

## Initial lineup

The plan for the time being, in no particular order, is:


* [ ] CMake
* [ ] Python + pip (bundled) 
* [x] Node + npm (bundle), replacing the 5 node version managers recommended by npm's npm package (Node/NPM needs to die in a fire)
* [x] Vim
* [ ] ssh/sshd
* [ ] Config for custom packages, that may or may not be completely unrelated to package managers and SDKs

However, an important point here is to also allow uninstalling. This isn't something that's easily done when installing manually, because it some times installs dependencies in a bunch of different places. This is primarily true for makefile-based programs.

That said, this project may evolve into a glorified "unstable apt" that sources the newest packages, instead of whatever ancient version retrieved when the dinosaurs were still around, that the repos push. We'll see how far this rabbit hole goes.

## How does it work?

Packages are installed to `/opt/upm`. Packages are then activated via symlinks in `/usr/local/`, similar to how a normal manual installation would work. 

To make a painfully long and already well-documented story short, installing third party packages in "non-standard" formats on Linux fucking sucks. "Non-standard" in this context really just means anything that doesn't fit the format of the system-wide package manager. And yes, this includes binary tars.

upm has gone through several iterations of different package locations, only to return to the starting point: `/usr/local`.

There are a few reasons for this, and there are probably many more than the reasons I'm aware of. Let's assume we want a non-standard install directory. This affects:

1. **Binary path**: new additions require amendments to the PATH variable. This isn't too bad, but there isn't an automated way to do this. For package managers, this is fine; you add one directory, and you're good to go. If you want to install individual packages though, it isn't as convenient.
2. **Include and library path**: this does not apply to all packages (by a long shot), but any packages that come with a development component need a discoverable include and library path. This is now two more variables.
3. **`.desktop` files**: not all packages are terminal tools. Some need desktop entries, or other metadata additions. In the case of desktop files in particular, there's two more categories:
    1. Relative bin path: #1 has to be set beyond the shell, complicating installation
    2. Absolute bin path: The desktop file has to be modified, either by the install process or the user to function, at least if the install location is unexpected.
4. **man pages**: Many packages include man pages, and these also require a variable addition.

[1]: https://imgs.xkcd.com/comics/standards.png
