#!/usr/bin/env bash

make CC=${CROSS_COMPILE}gcc CXX=${CROSS_COMPILE}g++

g++ simpleZmqClient.cpp -o zmqClient -lzmq
