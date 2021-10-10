# Version syntax

Installation, uninstallation, and updates build around version markers. There's a few syntactical markers supported:

* `latest` - try to resolve the latest version and download it. Some packages may not support this
* `lts` - get the LTS version. Only possible for packages that have an LTS version, and that also have an easy way to determine what the LTS is

Some packages may have additional semantic markers. [TODO: document]

These are the supported package identifiers:

* `package@marker`, where `marker` is one of the syntactical markers mentioned earlier in this document.
* `package@version`, where `version` is a proper package version (such as 1.2.3, assuming it exists)
* `package~partialVersion`, where `version` becomes an upgrade marker. For an instance, this can be 1.x or 1.2.x, which is used to resolve updates.

    `x` is used as an alternative to `*`, because writing `*` in zsh is annoying (it's interpreted as a wildcard in the shell instead of in the program receiving it), though `*` is also accepted. For consistency, preferring `x` is recommended, however. 

    Strictly speaking, `package~x` is potentially an emulated `latest`, but it's not always supported due to how certain servers work.

