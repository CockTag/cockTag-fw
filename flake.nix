{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-22.05";
    nixpkgs-unstable.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils, nixpkgs-unstable }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          config.allowUnfree = true;
        };
        pkgs-unstable = import nixpkgs-unstable {
          inherit system;
          config.allowUnfree = true;
        };
        darwinPackages = if pkgs.stdenv.isDarwin then [
          pkgs.darwin.apple_sdk.frameworks.CoreServices
          pkgs.darwin.apple_sdk.frameworks.ApplicationServices
          pkgs.darwin.apple_sdk.frameworks.Security
        ] else
          [ pkgs-unstable.nrf-command-line-tools
        ];
        nrf5-sdk = (pkgs.callPackage ./nix/nrf5.nix { });
      in {
        devShells = {
          default = pkgs.mkShell {
            buildInputs = with pkgs;
              [
                cmake
                ninja
                nrf5-sdk
                gcc-arm-embedded-6

              ] ++ darwinPackages;
            shellHook = ''
              export SDK_ROOT=${nrf5-sdk.outPath}/share/nRF5_SDK
            '';
          };
        };
      });
}
