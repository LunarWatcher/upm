# Upstream Package Manager (upm)

Package manager manager and SDK manager, because someone had to make the 15th standard.

[![xkcd: standards][1]][1]

## Requirements

A C++17 compiler, and Linux or MacOS, and access to `tar` with a few specs (see the tar section). 

Note that MacOS will have weaker support, because I don't have access to any Mac machines. Please consider contributing to the project if you have access to a Mac and want to use upm.

General UNIXes beyond Linux and MacOS will have varying support, for the same reason MacOS doesn't have full support: I don't have those operating systems, and testing them is a slow and annoying process that I don't want to spend my time on.

### Additional libraries (Linux)

* `libssl-dev`


### Tar

* Supports `--strip-components`
* Supports automatic tar type detection

### Package dependencies

Certain packages may have other dependencies. If such a dependency is required, and the build doesn't self-manage dependencies, it'll trigger an error if it isn't found. 

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

## Installing upm

### Using a utility script

Upm provides an installer utlity that runs all the required compile commands automatically. Also note that the script does **not** handle dependency install. You need to manually get the dependencies before running.

**Note:** it's always recommended to read through scripts before running them.

```bash
sh -c "$(wget -O- https://raw.githubusercontent.com/LunarWatcher/upm/master/tools/install.sh)"
```

### Manually

```bash
# Clone the repo
git clone https://github.com/LunarWatcher/upm
# cd and and build the bootstrap
cd upm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release 
make -j $(nproc)
# Have the upm bootstrap install itself
sudo ./bin/upm install upm@nightly --install_self=true
```

After this, you can delete the cloned repo. When upm later updates itself, the repo will be automatically re-cloned under /tmp for building.

### Updating upm

When upm has been installed, upm can update itself. Simply run `sudo upm upgrade upm@nightly` (or another tag if you prefer), and it'll fetch itself.

[1]: https://imgs.xkcd.com/comics/standards.png
