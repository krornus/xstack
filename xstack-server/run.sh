#!/bin/sh

make
./xstack &
sleep 5
killall xstack
