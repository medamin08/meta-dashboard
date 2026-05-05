#!/bin/sh
# Setup virtual CAN interface
modprobe vcan
ip link add dev vcan0 type vcan
ip link set up vcan0
echo "vcan0 interface is up"
