# Git
```lua
require "git"
```

## `clone(repo, dest, clean = false, absPath = false)` => `success: bool, repoPath: string`

| Param | Type | Description |
| --- | --- | --- |
| repo | string | URL to the Git repo to clone |
| dest | string | The name to clone into |
| clean | bool | Whether or not to delete the destination folder if it exists.
| absPath | bool | If false, `dest` is postfixed to `/tmp/upm/`. If true, `dest` is treated as an absolute path. It's up to the caller to ensure `dest` is valid, and placed somewhere valid and easily cleanable. For the vast majority of plugins, this value should be left as the default value of `nil`. |

## `checkout(repoLoc, checkoutObj, masterMainFutureProofing = true)`

| Param | Type | Description |
| --- | --- | --- |
| repoLoc | string | The filesystem path to the repo. Validity is not checked prior to running the commands, as it's expected to be checked by the caller. |
| checkoutObj | string | What thing to pass to `git checkout`. Can be a branch or a commit, but it must exist.|
| masterMainFutureProofing | bool | Whether or not to future-proof against repos switching between master and main. If true and the checkout fail, and the checkoutObj is master, main is attempted before considering the checkout a failure. Equivalently, if checkoutObj is main, master is tried before considering the checkout a failure. This is to avoid having to change the code just because the primary branch changes. Should be set to false if both the master and main tags exist with a different purpose. If not, it's recommended to leave as the default value of true |

## `pull(repoLoc)`

| Param | Type | Description |
| --- | --- | --- |
| repoLoc | string | The filesystem path to the repo. Validity is not checked prior to running the commands, as it's expected to be checked by the caller. |

Wrapper around `git pull $(git remote) $(git rev-parse --abbrev-ref HEAD)`. Note that it's not checked whether or not the repo is on a branch or in a detached state. if this can't be verified, the invoker should run `checkout` in advance to ensure the right thing is pulled.
