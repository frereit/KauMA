#!/bin/sh
make clean
make release
sudo chrt -f 99 perf stat -r 10 -ddd out/apps/kauma $@ >/dev/null
make clean
make debug
valgrind --tool=callgrind out/apps/kauma $@
