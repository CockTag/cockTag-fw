{}:

let
    pkgs = import(builtins.fetchTarball "channel:nixos-22.05") {};
    nrf5-sdk = (pkgs.callPackage ./nrf5.nix {});
    darwinPackages =  if pkgs.stdenv.isDarwin then [ 
        pkgs.darwin.apple_sdk.frameworks.CoreServices 
        pkgs.darwin.apple_sdk.frameworks.ApplicationServices 
        pkgs.darwin.apple_sdk.frameworks.Security ] else [];
in
pkgs.mkShell {
    buildInputs = with pkgs; [
	    cmake
	    ninja
	    nrf5-sdk
	    gcc-arm-embedded
    ]
    ++ darwinPackages;
    shellHook = ''
        export SDK_ROOT=${nrf5-sdk.outPath}/share/nRF5_SDK
    '';
}
