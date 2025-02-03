#!/bin/bash

g++ -std=c++20 -Wall -Wextra -Werror -Iinclude test/Connection.test.cpp src/http/*.cpp src/utils/*.cpp
./a.out