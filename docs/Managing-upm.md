# Managing upm

This document covers installing, uninstalling, and updating upm.

## Dependencies

* Git
* CMake (3.6 or newer)
* Python 3 with support for `python3 -m venv env`

Upm itself does have additional dependencies, but prefers to manage these on its own, as well as through conan. The direct dependencies are listed in the top-level CMakeLists, if you're interested.

## Installing

### Manually

Manually installing the upm is standard procedure; clone, download a zip of, or otherwise download the source code (available on [GitHub](https://github.com/LunarWatcher/upm)) into somewhere you feel makes sense, and `cmake .. && make && sudo make install` when you've ensured you have the appropriate dependencies.

### Install scripts

Upm comes with an download and install script that downloads the source code, sets up the venv, compiles, and installs upm.

There's also a separate installation script that _only_ installs upm, and sets up the environment.

## Installing an experimental version

TODO after writing install scripts

## Uninstalling

TODO
