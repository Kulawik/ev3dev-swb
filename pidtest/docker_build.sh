#!/bin/bash

help() { echo -e "Usage\n\t$0: new <app_name> [-b <develop>]" >&2;}
die() { [ -n "$1" ] && echo -e "Error: $1\n" >&2; help; [ -z "$1" ]; exit;}

[ $# -lt 1 ] && die "Too few args"
app=$1
shift 1

build="build"
while getopts "b:" opt; do
  case $opt in
    \?) exit 1;;
    :) exit 1;;
    b) build="$OPTARG";;
  esac
done

docker run --rm -it -v $app:/home/compiler/app_ ev3cc /bin/bash -c "cd app_ && mkdir -p $build && cd $build && rm -rf * && cmake .. \$(~/toolchain.sh) && make"
