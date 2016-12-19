#!/bin/bash

help() { echo -e "Cross-compile cmake-based project for ev3dev\nUses docker image 'ev3cc'\nUsage\n\t$0: <path_to_project> [-b <build directory (default: build)>]" >&2;}
die() { [ -n "$1" ] && echo -e "Error: $1\n" >&2; help; [ -z "$1" ]; exit;}

[ $# -lt 1 ] && die "Too few args"
app=$(readlink -f $1)
shift 1

if [ ! -d "$app" ]; then
  echo -e "No such directory: $app";
  exit 1;
fi

build="build"
while getopts "b:" opt; do
  case $opt in
    \?) exit 1;;
    :) exit 1;;
    b) build="$OPTARG";;
  esac
done

docker run --rm -it -v $app:/home/compiler/app_ ev3cc /bin/bash -c "cd app_ && mkdir -p $build && cd $build && rm -rf * && cmake ..  -DCMAKE_TOOLCHAIN_FILE=/home/compiler/toolchain-armel.cmake && make"
