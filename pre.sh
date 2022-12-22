#!/usr/bin/env bash

cmake . && cmake --build .
rm -rf ./cmake_install.cmake ./CMakeCache.txt ./cmake-build-debug ./CMakeFiles

killall Xephyr
Xephyr :1 &
sleep 0.5s

DISPLAY=:1 picom --experimental-backend &
DISPLAY=:1 xwallpaper --stretch ~/wallpapers/wallhaven-o59gvl.jpg &

DISPLAY=:1 ./Ace