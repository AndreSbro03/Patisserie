#!/bin/sh

test_output(){
  NOME=$1 
  ./build/$NOME < Tests/"open$2.txt" > myout.txt 
  sdiff myout.txt Tests/"open$2.output.txt";
}

test_differenze(){
  NOME=$1 
  ./build/$NOME < Tests/"open$2.txt" > myout.txt;
  sdiff -l myout.txt Tests/"open$2.output.txt" | cat -n | grep -v -e '($';
}

"$@"
