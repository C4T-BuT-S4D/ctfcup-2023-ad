#!/bin/bash
set -e

mkdir build
cd build && cmake .. && make && cd ..
mv build/app ./app
mv build/station ./station
