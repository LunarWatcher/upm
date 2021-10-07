# Uninstalling a package

NOTE: This document does NOT cover uninstalling upm itself. See \[document to be determined at a later time\] for instructions on the various options for uninstalling upm.

## Practical

`upm uninstall package@version` lets you uninstall a specific version of a package. `upm uninstall package@all` uninstalls every version of the package. The syntax for this is explicit to make sure you don't accidentally uninstall every version when you just want one. (The alternative is a prompt, which is annoying for everyone involved.)

## Technical
Uninstallation of packages is a two-step process, though it's automated internally.

When a package is installed, it ends up in its own directory, denoted by `packageName-version`. This is how each installed version is indentified for internal use surrounding activation [TODO: write activation docs], and consequently deactivation.

Uninstallation first involves the package being deactivated, becase we don't want any dead symlinks. Then, the package is removed from the system, by removing the appropriate folder.


