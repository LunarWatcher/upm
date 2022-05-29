# How it works

**NOTE:** At this point in time, this is a theoretical explanation of how it'll eventually work, and doesn't account for unforeseen obstacles that change what has to be done.

All activated programs are added as symlinks in /opt/upm-active, which is equivalent to /usr/local; however, with the major exception that /usr/local isn't usually a part of the PATH. Relevant directories have to be added here. It's unclear whether this affects library installs, if this is added in the future.

In either case, the binaries are able to find their runtime dependencies and work.

Where possible, binaries are preferred to source code to avoid unnecessary building. We have pre-built binaries for a reason.
