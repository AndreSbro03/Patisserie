#!/bin/sh

file(){
  set -xe

  CFLAGS="-Wall -Wextra -std=gnu11 -O2 -g3"
  DEBUG="-fsanitize=address"
  #DEBUG=""
  DFLAGS="-lm" #-lX11
  NAME=$1

  gcc $CFLAGS $DEBUG -o $NAME src/$NAME.c $DFLAGS
  mv $NAME ~/Desktop/API/progetto/build/
}

massif(){
  valgrind --tool=massif ./build/App < Tests/open7.txt 
  mv massif.out.* massif_07
  valgrind --tool=massif ./build/App < Tests/open8.txt 
  mv massif.out.* massif_08
  valgrind --tool=massif ./build/App < Tests/open9.txt 
  mv massif.out.* massif_09
  valgrind --tool=massif ./build/App < Tests/open10.txt 
  mv massif.out.* massif_10
  valgrind --tool=massif ./build/App < Tests/open11.txt 
  mv massif.out.* massif_11
}

callgrind(){
  valgrind --tool=callgrind ./build/App < Tests/open7.txt 
  mv callgrind.out.* callgrind_07
  valgrind --tool=callgrind ./build/App < Tests/open8.txt 
  mv callgrind.out.* callgrind_08
  valgrind --tool=callgrind ./build/App < Tests/open9.txt 
  mv callgrind.out.* callgrind_09
  valgrind --tool=callgrind ./build/App < Tests/open10.txt 
  mv callgrind.out.* callgrind_10
  valgrind --tool=callgrind ./build/App < Tests/open11.txt 
  mv callgrind.out.* callgrind_11
}

"$@"
