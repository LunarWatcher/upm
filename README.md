# Upstream Package Manager (upm)

Package manager manager and SDK manager, because someone had to make the 15th standard.

[![xkcd: standards][1]][1]

---

## What?

Installs shit so you don't have to, and manages the dependencies of the dependencies so you're not stuck with version mismatches.

## Like?

Whatever has been set up.

## How?

Through a combination of GitHub repositories, automatic redirection, and good 'ol fashioned manual link following. And of course lists for when absolutely everything else fails.

## Requirements?

A C++17 compiler, and Linux or MacOS, and access to `tar` with a few specs (see the tar section). Note that MacOS will have weaker support, because I don't have access to any Mac machines. Please consider contributing to the project if you have access to a Mac.

General UNIXes beyond Linux and MacOS will have varying support, for the same reason MacOS doesn't have full support: I don't have those operating systems, and testing them is a slow and annoying process that I don't want to spend my time on.

Additionally, while not a requirement per se, the PATH has to be augmented to add /opt/upm-active/bin, and possibly others. /bin is required for binaries. It's currently unclear how any of this affects libraries.

### Tar

* Supports `--strip-components`
* Supports automatic tar type detection*

\*: may be dropped if it turns out there's a lot of tars that support `--strip-components`, but somehow not automatic type detection. Feedback needed.

## Initial lineup

The plan for the time being, in no particular order, is:


* [ ] Maven
* [ ] Sdkman (maven is supported separately from sdkman, but Java isn't, because of the substantially better repo support. Java also has more platform quirks)
* [ ] CMake
* [x] Python + pip (bundled)
* [x] Node + npm (bundle), replacing the 5 node version managers recommended by npm's npm package
* [ ] Vim
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

* **"Containerized" packages:** cross-environment reproducability isn't a goal for this package manager, at least at this time. I suggest using the packages you install with upm to get containers, such as pyenv. This may change if I actually figure out how package managers like that work, and decide to actually do it, which is very unlikely at the time of writing.
* **Registry of pre-built packages:** while it's convenient to just download pre-built packages, I have no way to host them, and I've had enough messing around with APIs and server infrastructure for the near foreseeable future, even if I had a way to host it.


[1]: https://imgs.xkcd.com/comics/standards.png
