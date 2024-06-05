#!/bin/sh

set -xe

CFLAGS="-Wall -Wextra -std=gnu11 -O2 -g3"
#DEBUG="-fsanitize=address"
DFLAGS="-lm" #-lX11
NAME="App"

gcc $CFLAGS -o $NAME main.c $DFLAGS
mv $NAME Build/
