#!/bin/bash

#sudo make uninstall
make distclean

autoreconf --install || exit 1

./configure --prefix=/usr/local

cd po
make update-po

cd ..
make
#sudo make install
