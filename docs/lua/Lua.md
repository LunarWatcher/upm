# The Lua API

upm uses Lua to define packages, as well as their activation systems. To achieve this, there's an extensive API, documented in other files in this folder.

This document serves as an introduction to the general systems used by upm, i.e. the skeleton required to make a functioning package script, and not individual package functions.

## Current Lua version: 5.4

## General script structure

You're free to structure your scripts however you see fit<sup>[1]</sup>. However, to aid with identification of the core methods, every script has to return a table with fixed keys. One example from the Vim script:

```lua
return {
    install = install,
    versionValidator = vResolvers.validateGitVPrefix,
    apply = activators.universalUNIX
}
```

These are, incidentally, all the required keys, as well as some optional ones. The values for `apply` and `versionValidator` are API functions, discussed in other documents. However, in short:

* `vResolvers` is a library (`local vResolvers = require "vResolvers"`), and primarily does version resolution, with a bit of version validation on the side. Its validation suite is generally associated with resolvers by the same name. `validateGitVPrefix`, for instance, is used with its `git(repoPath)` resolver function. `Git` symbolises it validates in line with the capabilities of the `git` version resolver, and `VPrefix` means it does  so in a way that also looks for a prefixed `v` in front of the version.
* `activators` is a library (`local activators = require "activators"`), and defines a bunch of default activation systems. Most critically, `activators.universalUNIX`, which activates all packages existing only in the default UNIX format (i.e. `bin`, `lib`, `include`, ...). This is, by far, the most commonly used activator in the entire library.

To see more version resolvers and activators, and more in-depth and technically explanations, see their respective documentation.

[1]: ... in theory. Contributions to the core scripts have higher requirements, and specific standards written by me because I can. You're free to write them however you see fit in your own package repositories, but that doesn't mean individual package repositories can't have their own standards for script structure outside the required bare minimum. Your own package repos can define its own standards, as long as they follow the minimum requirements. There's no general style decisions or function name requirements, nor a requirement only so many functions can be included. Use whatever general script structure you feel like, and it's fully irrelevant as long as the correct functions are supplied in the return value from the script, and as long as it's valid Lua 5.4.

### `install`

In an optimal world, there would we a lot clearer structure in everything.

However, packages are messy, and I've therefore decided that it's more convenient to use a unified installation function, rather than splitting it up, particularly as some pre-install functions may need to have parameters passed on to the install function. It's just awkward.

Instead, this puts the job on the individual script to define certain things.

As mentioned in the documentation for installing packages (usage docs, not developer docs), there's several semantic versions defined that packages may choose to provide.

Critically for `install` is the `ctx` global object (no import required), as well as the large array of utility libraries. For all intents and purposes, the `ctx` global object is what you want to access to get any kind of input data; this includes the package version (both resolved and raw), access to the install prefix if necessary, configuration options (if any), and some defined command line arguments with potential flags.

`ctx` is also used to automate critical functions, but that can come in different parts of the install process itself:

* Setting the resolved version, if a version resolver isn't used
* Checking for existing versions, and checking the `--reinstall` parameter to determine whether or not to erase the existing install or abort

### `apply`

This function is called during the application process of a package. This key overwhelmingly gets a value equivalent to an existing activator (see the activators documentation).

When it doesn't, it uses `ctx` to get the install prefix, and perform the symlinks and any necessary directory creation. This function should NOT use `upmfilesystem` for any write processes. `upmfilesystem` may be performed to, among other things, iterate directories and files, but `ctx` has to be used to apply the binaries.

This is a requirement to ensure automatic disabling, and by extension, uninstallation. upm's context API automatically tracks and stores symlinks in its config file, to save time dealing with uninstallations.

### `versionValidator` (optional)

This (optional) function lets `upm` validate the form of the version prior to installation. This function can perform validity checks, but it's primarily intended to check the syntax. Take `vResolvers.validateGitVPrefix`; all it does is ensure `version == "latest" || version == "nightly" || version =~ "v.*"`. This means it could pass `v6.9`, but whether that version _exists_ will not be resolved in that step.

In essence, this function exists to ensure early errors for syntactic mistakes. Whether that version exists is dealt with later.
