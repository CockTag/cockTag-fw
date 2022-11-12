{}:

let
    pkgs = import(builtins.fetchTarball "channel:nixos-22.05") {};
    darwinPackages =  if pkgs.stdenv.isDarwin then [ 
pkgs.darwin.apple_sdk.frameworks.CoreServices 
pkgs.darwin.apple_sdk.frameworks.ApplicationServices 
pkgs.darwin.apple_sdk.frameworks.Security ] else [];
in
pkgs.mkShell {
    buildInputs = with pkgs; [
	cmake
	ninja
	(pkgs.callPackage ./nrf5.nix {})	
	gcc-arm-embedded
    ]
    ++ darwinPackages;
}
