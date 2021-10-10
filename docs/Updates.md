# Package updates

Package updates aren't always supported; it depends on the specific markers used, as outlined in [the syntax document](Syntax.md).

Essentially: only specific markers are supported. `package@version` does not support updates, because upm has no idea what to update to, and much less if you even _want_ to update. Versions installed at an _exact_ version point cannot be automatically updated. There's a shorthand update syntax that's equivalent to uninstalling, installing, and activating for these packages, or packages using dead package markers: `package@oldVersion:newVersion`

For markers that support rolling updates, however (including `latest`, `lts`, and the `package~partialVersion` syntax), these are automatically resolved to check if there are updates available.

Simply put: if you always want the latest version of a package to be installed, use `latest`. If you want an LTS version, use `lts`. If you want to follow a major version, `1.x` (where `1` represents the major version you're interested in) is what you want.
