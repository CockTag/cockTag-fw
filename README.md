# cockTag-fw
firmware for nRF51 chips that is coincidentally compatible with the FindMy (AirTag) ecosystem.

It sends out advertisement keys defined in [keys.h](keys.h), rotating keys every hour.

## Setup
```
nix develop --extra-experimental-features nix-command --extra-experimental-features flakes
mkdir build
cd build
cmake .. -GNinja
```

## Build
```
cd build
ninja
```

## Flash using JLink
```
cd build
cmake --build . --target nrfjprog_flash_softdevice
cmake --build . --target nrfjprog_flash
```

## Flash using GDB
```
export GDB_REMOTE=/dev/ttyUSB0 # BlackMagic Probe
export GDB_REMOTE=farpatch.local:2022 # Farpatch
cd build
cmake --build . --target gdb_flash_softdevice
cmake --build . --target gdb_flash
```

## Debug using GDB
```
export GDB_REMOTE=/dev/ttyUSB0 # BlackMagic Probe
export GDB_REMOTE=farpatch.local:2022 # Farpatch
./run_gdb.sh
```