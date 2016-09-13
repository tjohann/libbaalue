#!/usr/bin/env bash

echo "+------------------------------------------------------+"
echo "| Delete vcan0 node (need sudo)                        |"
echo "+------------------------------------------------------+"
sudo ip link set down vcan0
sudo ip link delete dev vcan0 type vcan

echo "+------------------------------------------------------+"
echo "| Cheers $USER "
echo "+------------------------------------------------------+"
