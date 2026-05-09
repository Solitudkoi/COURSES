#! /bin/bash
cd src
g++ main.cpp ad_var.cpp ad_graph.cpp -o main -std=c++20 
./main
cd ..