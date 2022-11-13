{ lib
, stdenv
, fetchzip
, pkgs
}:

stdenv.mkDerivation rec {
  pname = "nrf5-sdk";
  version = "12.3.0";

  urlHash = "d7731ad";

  src = fetchzip {
    url = 
"https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v12.x.x/nRF5_SDK_${version}_${urlHash}.zip";
    sha256 = "sha256-ybi2l998Pg/MijrBbIZAkbApv6WSNd+GvN0qoLu0/Ko=";
  };

  buildInputs = [ 
    pkgs.gcc-arm-embedded-6
  ];

  dontConfigure = true;
  dontBuild = true;

  installPhase = ''
    runHook preInstall
    mkdir -p $out/share/nRF5_SDK
    mv * $out/share/nRF5_SDK
    rm $out/share/nRF5_SDK/*.msi
    cat <<EOF > $out/share/nRF5_SDK/components/toolchain/gcc/Makefile.posix
GNU_INSTALL_ROOT := ${pkgs.gcc-arm-embedded-6}
GNU_VERSION := 6.3.1
GNU_PREFIX := arm-none-eabi
EOF
    runHook postInstall
  '';
  
  meta = with lib; {
    description = "Nordic Semiconductor nRF5 Software Development Kit";
    platforms = platforms.all;
  };
}
