# Context

The module consits of a single global object, named `ctx` by default. No imports are required.

## `getArch()` => `os: string, cpuArch: string`

Returns the OS and CPU architecture as identified at compile time.

If one value is set to UNK, both are, and the identification process failed. This is only really significant for binary packages, as the compilation process has far more advanced detection methods than upm, at the time of writing.

Possible values for `os`: `linux`, `darwin`, `UNK`
Possible values for `cpuArch`: `x64`, `armv7l`, `arm64`, `UNK`


## `checkInstalled()`

Checks whether or not the package has been installed. If it has been installed with the same version, and the `--reinstall` flag has been supplied, the package will automatically be removed. If the package has already been installed and the `--reinstall` flag isn't supplied, the function throws an error to abort the install process.

This function is silent, and doesn't return anything on success.

## Readable indices (`ctx[key]`)

| Key | Type | Description |
| --- | ------- | --- |
| package | string | Package name; generally always equal to the script name |
| version | string | The version input from the user |
| resolvedVersion | string | The resolved version; generally always an exact version |
| prefix | string | The package install prefix; can be passed directly to `make`, `cmake`, or similar tools. Note that the install prefix is set by default for several build tools. Contains the path to where the package should be installed. The prefix is internally calculated using version of resolvedVersion isn't set. The prefix should be retrieved after `resolvedVersion` is set to ensure the package isn't installed to one of the semantic markers (such as `package-latest`), as this can cause conflicts or lost packages. |

## Writeable indices (`ctx[key] = value`)

| Key | Type | Description |
| --- | --- | --- |
| `resolvedVersion` | string | For manual version resolvers, this key can be used to export the resolved version |
