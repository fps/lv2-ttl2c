nix-build -E 'with import <nixpkgs> {}; python3Packages.callPackage ./. {}'
