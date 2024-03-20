#!/bin/bash


build_and_run() {
    if [ ! -d "build" ]; then
        mkdir build
    fi
    cd build || exit
    cmake ..
    make -j"$(nproc)"
    notify-send "ATOM" "Run App."
    ./app/executable_headless
    cd ..
}



echo "No new commits."
notify-send "ATOM" "Begin build."
build_and_run

