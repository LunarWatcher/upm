# Developers

This document is only for people who intend on extending the code of upm, or who want to learn more about it. If you're looking to add custom packages (not to be confused with adding packages in general), or just to generally use it, this is not the file you're looking for.

## OS support

OS support is primarily done through macros, because I had a moment. The binaries aren't portable anyway, so might as well use that to our advantage.

Essentially, whereever there's a choice in binary type, and it's tied to the system itself, use macros instead of variables and runtime detection. We don't need to do that because we have all the info we need at compile time.

For an instance, node: we need to know the OS and the architecture. We can detect 64 bit linux, and use that to get the 64 bit pre-compiled binaries. Same with ARM, Mac (not supported at the time of writing), and use all of these as a fallback to download the source and build, if there are no available binaries.

The same holds elsewhere, and we can offload a decent chunk of this work to compile-time.

## The tricky part: updates

There's no central index. The way the Node upgrader works, for an instance, is by looking for the version on the download site.

Having a central index is still self-defeating, however. Unless the CI automates updates, it'll eventually suffer from growing out of date when I inevitably focus on other projects.

Some sort of version caching may become applicable in the future, at least when we stumble over projects where version extraction is harder.
