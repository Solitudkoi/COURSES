#! /bin/bash
cd src
g++ main.cpp arc.cpp circle.cpp edge.cpp line.cpp sketch.cpp vertex.cpp -o main -std=c++20 
./main
cd ..