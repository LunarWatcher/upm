# How it works

**NOTE:** At this point in time, this is a theoretical explanation of how it'll eventually work, and doesn't account for unforeseen obstacles that change what has to be done.

On all distros I'm aware of, /usr/local/bin is prioritized over /usr/bin. This has the fantastic benefit of allowing Gamma to work. Gamma handles symlinks in /usr/local/bin to a set of binaries installed in `/usr/local/bin/gamma`.

At the time of writing, no systems are in place for dealing with shebang programs using `/usr/bin/<some program>`, though I believe this is primarily a Python problem. This needs to be addressed in the future.

Where possible, binaries are preferred to source code to avoid unnecessary building. We have pre-built binaries for a reason.
