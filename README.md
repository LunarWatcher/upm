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

### Tar

* Supports `--strip-components`
* Supports automatic tar type detection*

\*: may be dropped if it turns out there's a lot of tars that support `--strip-components`, but somehow not automatic type detection. Feedback needed.

## Initial lineup

The plan for the time being, in no particular order, is:

* OpenJDK
* Proprietary JDK
* Maven
* ... or possibly managing sdkman if implementing these are a pain
* CMake
* Python + pip (possibly separate)
* Node + npm (bundle), replacing the 5 node version managers recommended by npm's npm package
* Config for custom packages, that may or may not be completely unrelated to package managers and SDKs

However, an important point here is to also allow uninstalling. This isn't something that's easily done when installing manually, because it some times installs dependencies in a bunch of different places. This is primarily true for makefile-based programs.

That said, this project may evolve into a glorified "unstable apt" that sources the newest packages, instead of whatever ancient version retrieved when the dinosaurs were still around, that the repos push. We'll see how far this rabbit hole goes.

## Advantages

* One package manager
* ... that's self-managed
* Exposes binaries through already existing entries in the PATH: no performance loss (unlike nvm, which takes at least 2.6+ seconds to load, blocking my terminal in the process)
* Manages symlinks, not path entries, and is always on-demand invoked

## Disadvantages

* Still doesn't modify /usr/bin packages, and consequently may break with programs that hard-code `/usr/bin/<binary>`-paths. This is currently [wontfix] because of the potentially bad side-effects this has.

[1]: https://imgs.xkcd.com/comics/standards.png
