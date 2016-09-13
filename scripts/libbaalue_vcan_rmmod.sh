#!/usr/bin/env bash

echo "+--------------------------------------------------------+"
echo "| Un-load vcan driver -> prepare your password (for sudo)|"
echo "+--------------------------------------------------------+"
sudo rmmod vcan
sudo rmmod can_bcm
sudo rmmod can_raw
sudo rmmod can_dev
sudo rmmod can

echo "+--------------------------------------------------------+"
echo "| Cheers $USER "
echo "+--------------------------------------------------------+"
