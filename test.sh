#!/bin/sh

test_out(){
  NOME=$1 
  ./build/$NOME < Tests/"open$2.txt" > myout.txt 
  sdiff myout.txt Tests/"open$2.output.txt";
}

test_diff(){
  NOME=$1 
  ./build/$NOME < Tests/"open$2.txt" > myout.txt;
  sdiff -l myout.txt Tests/"open$2.output.txt" | cat -n | grep -v -e '($';
}

"$@"
