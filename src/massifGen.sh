#!/bin/sh

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

