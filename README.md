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

Additionally, while not a requirement per se, the PATH has to be augmented to add /opt/upm/active/bin, and possibly others. /bin is required for binaries. It's currently unclear how any of this affects libraries.

### Tar

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

## Advantages

* One package manager
* ... that's self-managed, and running on UNIX, a place where binaries can modify the underlying binary without needing to stop.
* Exposes binaries through already existing entries in the PATH: no performance loss (unlike nvm, which takes at least 2.6+ seconds to load, blocking my terminal in the process)
    * ... and still offers version management
* Manages symlinks, not path entries, and is always on-demand invoked

## Disadvantages

* Still doesn't modify /usr/bin packages, and consequently may break with programs that hard-code `/usr/bin/<binary>`-paths. This is currently [wontfix] because of the potentially bad side-effects this has.

## Caveats

* While upm does handle version management, it's not meant to be a replacement for virtualenvs in your favorite language. This means that if you need a per-project install, this is currently outside the scope of upm. Note, however, that upm does let you set and use a specific version of a given package for a terminal session.
* At this time, upm uses its entirely own bin/lib/etc for managing active files. This isn't great either, but this is done due to the built-in safeguards preventing non-symlink overrides that may break the system. This shouldn't directly break much.

## Non-goals

* **"Containerized" packages:** cross-environment reproducability isn't a goal for this package manager, at least at this time. I suggest using the packages you install with upm to get containers, such as pyenv. This may change if I actually figure out how package managers like that work, and decide I like pain enough to actually do it, which is very unlikely at the time of writing.
* **Registry of pre-built packages:** while it's convenient to just download pre-built packages, I have no way to host them, and I've had enough messing around with APIs and server infrastructure for the near foreseeable future, even if I had a way to host it.

## Why not use [my favorite package manager]?

Why use mine. Use whatever works.

If you're considering using upm, that's probably a sign your current solution doesn't work. For the majority of people anyway, looking for a solution to a problem they've already solved in a satisfactory way is a waste of time. Don't get me wrong, I enjoy playing with stuff as much as the next person, but if I actively look for specific things acting as a replacement to somethign I have, I'm never happy with the existing solution.

There are probably hundreds of alternatives out there. Some include:

* [asdf](//asdf-vm.com)
* [nix](//nixos.org)
* [Homebrew](https://brew.sh/)
* ... and an [unbelievably large array of language-specific version managers](https://github.com/bernardoduarte/awesome-version-managers)

Upm exists as an alternative to these, offering:

* A name that isn't in [9th place for the most commonly used passwords](https://www.welivesecurity.com/2019/12/16/worst-passwords-2019-did-yours-make-list/#tablepress-790)
* Self-contained systems: the core scripts are bundled with upm
* When third party scripts are added, centralisation support is a goal; rather than the model of one repo per script, one repo is a package repo that can contain anywhere from 1 script to however many scripts your disk can fit.
* Version management without a focus on reproducibility: while potentially a drawback for many, there are far better alternatives if this is the goal. Notably Docker, which isn't a package manager, but if you want a universal environment, you literally cannot beat a systemt based on virtual machines. As previously mentioned, asdf and nix are also options, both of which have project reproducibility at the core of their goal. upm, on the other hand, exists to cover the land between a system package manager, and versioned package management; the ability to get up-to-date packages, or just installing some obscure version because you can or just need it briefly.
* Near 0 runtime overhead: all binaries are exposed via a single location that can be trivially added to the PATH. Some shell utilities for upm do get installed, but unlike certain version managers (*cough cough [nvm](https://github.com/nvm-sh/nvm)*), it won't slow your shell to a halt
* QOL extensions for scripts: the scripts being written in Lua means any system calls are defined via an API, meant to be as extensive as possible to eliminate unnecessary code for standardized systems.

[1]: https://imgs.xkcd.com/comics/standards.png
