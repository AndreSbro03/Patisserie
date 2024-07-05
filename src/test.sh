#!/bin/sh

test_output(){
  ./build/App < Tests/"open$1.txt" > myout.txt 
  sdiff myout.txt Tests/"open$1.output.txt";
}

test_differenze(){
  ./build/App < Tests/"open$1.txt" > myout.txt;
  sdiff -l myout.txt Tests/"open$1.output.txt" | cat -n | grep -v -e '($';
}

"$@"
