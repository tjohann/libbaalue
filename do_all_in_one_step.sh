#!/bin/bash

#sudo make uninstall
make distclean

autoreconf --install || exit 1

./configure --enable-debug --enable-examples --enable-scripts --disable-silent-rules --prefix=/usr --sysconfdir=/etc --libdir=/usr/lib

cd po
make update-po

cd ..
make
#sudo make install
