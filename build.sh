#!/bin/sh

file(){
  set -xe

  CFLAGS="-Wall -Wextra -std=gnu11 -O2 -g3"
  if [[ $2 == 0 ]]; then
    DEBUG=""
  else
    DEBUG="-fsanitize=address"
  fi

 

  if [[ $1 == 'main' ]]; then
    #gcc -o build/bst.o -c src/bst.c
    gcc -o build/algoritmi.o -c src/algoritmi.c
    DFLAGS="build/*.o -lm" #-lX11
  else
    #rm build/*.o
    DFLAGS="-lm"
  fi
  
  NAME=$1
  
  gcc $CFLAGS $DEBUG -o $NAME src/$NAME.c $DFLAGS
  mv $NAME ~/Desktop/API/progetto/build/
}

massif(){
  NAME=$1
  valgrind --tool=massif ./build/$NAME < Tests/open7.txt 
  mv massif.out.* massif_07
  valgrind --tool=massif ./build/$NAME  < Tests/open8.txt 
  mv massif.out.* massif_08
  valgrind --tool=massif ./build/$NAME  < Tests/open9.txt 
  mv massif.out.* massif_09
  valgrind --tool=massif ./build/$NAME  < Tests/open10.txt 
  mv massif.out.* massif_10
  valgrind --tool=massif ./build/$NAME  < Tests/open11.txt 
  mv massif.out.* massif_11
}

callgrind(){
  NAME=$1
  valgrind --tool=callgrind ./build/$NAME  < Tests/open7.txt 
  mv callgrind.out.* callgrind_07
  valgrind --tool=callgrind ./build/$NAME  < Tests/open8.txt 
  mv callgrind.out.* callgrind_08
  valgrind --tool=callgrind ./build/$NAME < Tests/open9.txt 
  mv callgrind.out.* callgrind_09
  valgrind --tool=callgrind ./build/$NAME < Tests/open10.txt 
  mv callgrind.out.* callgrind_10
  valgrind --tool=callgrind ./build/$NAME < Tests/open11.txt 
  mv callgrind.out.* callgrind_11
}

"$@"
