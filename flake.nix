{
  description = "A flake to build git-commits-bullet-hell";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs =
    {
      self,
      nixpkgs,
      ...
    }:
    let
      pkgs = import nixpkgs { system = "x86_64-linux"; };
    in
    {

      packages.x86_64-linux.git-commits-bullet-hell = pkgs.stdenv.mkDerivation (finalAttrs: {
        pname = "git-commits-bullet-hell";
        version = "0.0.0";

        src = ./.;

        nativeBuildInputs = with pkgs; [
          cmake
          xxd
        ];
        buildInputs = with pkgs; [
          git
          sdl3
          sdl3-image
          sdl3-ttf
        ];

        configurePhase = ''
          cmake -B build
        '';

        buildPhase = ''
          cmake --build build
        '';

        installPhase = ''
          mkdir -p $out
          cmake --install build --prefix $out
        '';

        meta = {
          description = "git log but its undertale pacifist ending credits";
          homepage = "https://github.com/p1k0chu/git-commits-bullet-hell";
          license = pkgs.lib.licenses.mit;
          maintainers = {
            github = "p1k0chu";
          };
        };
      });

      packages.x86_64-linux.default = self.packages.x86_64-linux.git-commits-bullet-hell;

    };
}
