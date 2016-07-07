#!/bin/bash

gcc xstack-msg/xstack-msg.c -o xstack-msg/xstack-msg
cd xstack-server
make clean
make

