#!/usr/bin/env bash

export CC=${CROSS_COMPILE}gcc
export CXX=${CROSS_COMPILE}g++

make


g++ simpleZmqClient.cpp -o zmqClient -Icppzmq -lzmq
