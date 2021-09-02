# Project Gamma

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

A C++11 compiler.

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

That said, this project may evolve into a glorified "unstable apt", that sources the newest packages, instead of whatever ancient version retrieved when the dinosaurs were still around, that the repos push. We'll see how far this rabbit hole goes.

[1]: https://imgs.xkcd.com/comics/standards.png
