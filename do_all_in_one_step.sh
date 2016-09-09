#!/bin/bash

#sudo make uninstall
make distclean

autoreconf --install || exit 1

#./configure --prefix=/usr/local
./configure CFLAGS='-O2' --enable-debug=yes --prefix=/usr --sysconfdir=/etc --libdir=/usr/lib

# disable examples
# ./configure CFLAGS='-g -O2' --disable-examples --prefix=/usr --sysconfdir=/etc --libdir=/usr/lib

cd po
make update-po

cd ..
make
#sudo make install
