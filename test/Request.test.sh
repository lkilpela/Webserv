#!/bin/bash

g++ -std=c++20 -Iinclude test/Request.test.cpp src/http/*.cpp src/utils/*.cpp
./a.out