#!/bin/sh

set -xe

CFLAGS="-Wall -Wextra -std=gnu11 -O2 -g3"
DEBUG="-fsanitize=address"
#DEBUG=""
DFLAGS="-lm" #-lX11
NAME="App"

gcc $CFLAGS $DEBUG -o $NAME main.c $DFLAGS
mv $NAME Build/
