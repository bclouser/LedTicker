#!/usr/bin/env python

#
#   Weather update client
#   Connects SUB socket to tcp://localhost:5556
#   Collects weather updates and finds avg temp in zipcode
#

import sys
import zmq

#  Socket to talk to server
context = zmq.Context()
socket = context.socket(zmq.SUB)

print("Collecting updates from our server")
socket.connect("tcp://127.0.0.1:3001")

socket.setsockopt_string(zmq.SUBSCRIBE, "tickerData".decode('ascii'))




# Process 5 updates
for update_nbr in range(5):
    string = socket.recv_string()
    print "Received:"
    print string