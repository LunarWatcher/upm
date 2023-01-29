# Installing a package

Note: this document does NOT refer to installing upm. See [TODO: document upm installation]

Installing packages is done through `upm install package@version` or `upm install package~version`.

## Using `package@version`
There's a few syntactical markers supported for `@version`:

* `latest` - try to resolve the latest version and download it. Some packages may not support this
* `lts` - get the LTS version. Only possible for packages that have an LTS version, and that also have an easy way to determine what the LTS is
* `nightly` - the bleeding edge

Packages are allowed to define their own semantical markers, but it should always be preferred to use standard terminology. Packages are not required to support the three markers, and there's a number of reasons for this:

* Some packages don't have an LTS
* Some packages are completely unversioned
* Some packages don't have a nightly version, or have one complicated enough for it to not be worth supporting

Consequently, there isn't always an exact meaning to these. A fully unversioned package may use `@latest` to refer to the latest commit, or the latest branch, and `@nightly` to refer to a dev branch, or again, the most recent commit. However, this is an implementation detail.

Git repositories are guaranteed to use @latest for the latest tag, and @nightly for the latest commit, with a pseudo tag (or the hash if the repo is truly unversioned) to label the version. Git repos additionally don't have support for LTS, as there isn't a way to mark these.

See also the help for individual packages to list their capabilities and config.

`version` may also be a proper version `1.2.3`.

## Using `package~version`

For `package~version`, this is meant to provide support for partial versions. For instance, if you want to install Vim 8.2 (which is no longer supported; Vim 9.0 is the newest at the time of writing, so nightly and latest aren't valid solutions), but you have no idea what the latest patch of it is, you can use `upm install vim~8.2`. upm registers this version as a partial version, and does its best to match your query to the latest version. If you run this now, upm figures out you're looking for v8.2.5172, and installs it.

Note that due to the way certain matching systems work, this may provide partly unexpected results. For instance, if you want the latest version of Node 18.1.x, you'd have to use `upm install nodejs~18.1.`; note the trailing period. If you don't, it's at risk of matching `18.12.1`, because it's the most recent version caught by the implicit wildcard.

Unlike `package@version`, this syntax does not support any syntactical markers. You must provide a version number. That version number here does not have to be complete, as already discussed.

## Listing packages

TODO

