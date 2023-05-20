#!/bin/bash

# install gtest

sudo apt-get install build-essential cmake

git clone https://github.com/google/googletest.git

cd googletest
mkdir build
cd build
cmake ..
make
sudo make install