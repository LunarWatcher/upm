# Install scripts

As a shock to presumably no one, upm requires scripts to install packages. These are written in Lua. See (TODO) for the development guide on these scripts.

Long story short, upm bundles a whole bunch of these scripts because there isn't an official host for them and I'd like to reduce the probability GitHub tanks the script repo if the package manager happens to get popular at some point in the future.

## How do I use them?

The scripts bundled with upm are self-managed by the upm installation process. Basically, you don't have to worry too much about the script management. Upm also runs them automatically, so it's not like a "usual" installer where you download it, double-click (or `./` execute, possibly after a `chmod +x`), and it does its thing. The scripts are used as a part of upm's runtime, so they're not standalone.

You're still encouraged to read the scripts before using them. It's a good security practice to never trust scripts you didn't write, regardless of how trustworthy the source is or may seem.

## What about custom scripts?

Excellent question. Upm allows you to write both per-user and per-system custom scripts. These currently take precedence over built-in scripts, but my personal hope is that overriding won't happen in practice (not because it's bad, but because I'd like the scripts to be as exhaustive as possible)

This does theoretically mean it's possible to link GitHub repos up, but this isn't a supported feature at this time. This may be revisited in the future if a need should occur, but the central repository distributed with upm is still open for additional packages, as long as the software can be used by anyone.

## But I'd really like my own remote

You're welcome to add support for it yourself (though working that in is going to take work), but for now, sorry, not supported.

It's not an optimal situation, but it's a necessary evil for compatibility until I figure out exact specifics of what I can or can't do with file hosting.

Basically, as long as the central index is bundled alongside upm, the situation will remain suboptimal for people in need of custom remotes. Sorry.
