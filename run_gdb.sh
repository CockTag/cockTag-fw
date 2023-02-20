#!/bin/bash
GDB_REMOTE="${GDB_REMOTE:=farpatch:2022}"
arm-none-eabi-gdb -nx \
-ex 'set mem inaccessible-by-default off' \
-ex 'set confirm off' \
-ex "target extended-remote $GDB_REMOTE" \
-ex 'monitor version' \
-ex 'monitor swdp_scan' \
-ex 'monitor rtt enable' \
-ex 'attach 1' \
-ex 'run' 
