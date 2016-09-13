#!/usr/bin/env bash

echo "+------------------------------------------------------+"
echo "| Load vcan driver -> prepare your password (for sudo) |"
echo "+------------------------------------------------------+"
sudo modprobe can
sudo modprobe can_raw
sudo modprobe can_bcm
sudo modprobe can_dev
sudo modprobe vcan

echo "+------------------------------------------------------+"
echo "| Setup vcan0                                          |"
echo "+------------------------------------------------------+"
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0

echo "+------------------------------------------------------+"
echo "| Show link info for vcan0                             |"
echo "+------------------------------------------------------+"
ip link show vcan0

echo "+------------------------------------------------------+"
echo "| Cheers $USER "
echo "+------------------------------------------------------+"
