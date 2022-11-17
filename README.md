# cockTag-fw
firmware for nRF51 chips that is coincidentally compatible with the FindMy (AirTag) ecosystem.

It sends out advertisement keys defined in [keys.h](keys.h), rotating keys every hour.

## Nix develop
`nix develop --extra-experimental-features nix-command --extra-experimental-features flakes`

## Flash using GDB
`make clean && make -j8 && arm-none-eabi-gdb -nx --batch -ex 'target extended-remote 192.168.1.192:2022' -ex 'monitor swdp_scan' -ex 'attach 1' -ex 'load _build/nrf51822_xxab.hex' -ex 'compare-sections' -ex 'kill'`