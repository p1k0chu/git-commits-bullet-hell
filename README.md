# GitCommitsBulletHell

a small SDL3 app that mimics undertale's true pacifist ending credits with git commits from your repo

needs git executable in path, current work dir needs to be in a git repo

### This is wip!

no bullet patterns yet, all commits spawn in top right corner facing the player

### Build

On nix, running `nix run .` should be enough. Otherwise, read on...

This project uses [cbuild](https://github.com/p1k0chu/cbuild). You need to clone with submodules, then
in project's root run `./lib/cbuild/bootstrap.sh`, after this running
`./build` will compile the project

Dependencies: `xxd git cmake sdl3 sdl3-ttf sdl3-image libgit2` (or see `flake.nix`/`build.c` for up-to-date deps)

# License

This project is licensed under MIT license (see LICENSE file)

This project uses 3rd party free stuff:
- the font: you can find the font itself and its license in `font/` directory

