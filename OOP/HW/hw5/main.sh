#! /bin/bash
cd src
g++ main.cpp vector.cpp matrix.cpp gaussian_elimination.cpp -o main -std=c++23
./main
cd ..