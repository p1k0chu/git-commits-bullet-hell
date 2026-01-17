# GitCommitsBulletHell

a small SDL3 app that mimics undertale's true pacifist ending credits with git commits from your repo

needs git executable in path, current work dir needs to be in a git repo

### This is wip!

no bullet patterns yet, all commits spawn in top right corner facing the player

### Build

On nix, just run `nix run .`

otherwise, to build this project you need these dependencies: `xxd git cmake sdl3 sdl3-ttf sdl3-image` (or see flake.nix for up-to-date deps)


very standard process:
```sh
cmake -B build
cmake --build build
# run the program:
# ./build/git-commits-bullet-hell
```




# License

This project is licensed under MIT license (see LICENSE file)

This project uses 3rd party free stuff:
- the font: you can find the font itself and its license in `font/` directory

