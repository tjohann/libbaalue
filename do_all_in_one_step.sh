#!/usr/bin/env bash

sudo make uninstall
make distclean

autoreconf --install || exit 1

# ./configure --enable-debug-info --enable-examples --enable-scripts --enable-lcd160x --disable-silent-rules --prefix=/usr --sysconfdir=/etc --libdir=/usr/lib
./configure --enable-debug-info --enable-examples --enable-lcd160x

cd po
make update-po

cd ..
make
sudo make install
