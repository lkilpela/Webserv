#!/bin/bash

g++ -std=c++20 -Iinclude test/Response.test.cpp src/http/*.cpp src/utils/*.cpp
./a.out