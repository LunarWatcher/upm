# Reserved and standard paths

Upm uses a number of different paths for  various purposes. The following paths are used by upm

| Path | Purpose | Notes |
| --- | --- | --- |
| `/opt/upm/_sources` | Contains upm's git repo. Cloned automatically if it doesn't exist during the install process. | No other packages may override or use this folder. It's reserved for upm's code |
| `/opt/upm/caches` | Packages in need of data caching can create a directory at `/opt/upm/caches/<packageName>` to contain whatever needs to be cached. | It's highly recommended to use `/tmp` instead, even for git repositories, in part to reduce unnecessary storage space. Upm treats network connections as an effectively unlimited resource. The folder is also not created by default. Depending on the API functions used, it may be up to each script to ensure the folder exists. |
| `/opt/upm/active` | Contains the standard UNIX directories for binaries, libraries, and other shared resources. | Must not be used for anything other than symlinks created during the activation process. |
| `/opt/upm/packages` | Contains subdirectories in the form of `/opt/upm/packages/<packageName>-<version>`, and contains all the binary data. This is the install location for individual packages and their versions. The path is automatically supplied by `ctx["prefix"]`, though it can be calculated manually as well. | |
| `/opt/upm/.upmrc` | This is a file containing all of upm's config, including the path of activated files. This is automatically managed by upm, and must not be overwritten or manually written to. | |
