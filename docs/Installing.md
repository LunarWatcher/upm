# Installing a package

Note: this document does NOT refer to installing upm. See [TODO: document upm installation]

Installing packages is done through `upm install package@version`. There's a few syntactical markers supported:

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

## Listing packages

TODO

