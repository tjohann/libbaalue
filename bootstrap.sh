#!/usr/bin/env bash

set -e

if [ -f .git/hooks/pre-commit.sample -a ! -f .git/hooks/pre-commit ] ; then
        cp -p .git/hooks/pre-commit.sample .git/hooks/pre-commit && \
        chmod +x .git/hooks/pre-commit && \
        echo "Activated pre-commit hook."
fi

echo "Setting up autotools"
autoreconf --install --force || exit 1

libdir() {
        echo $(cd $1/$(gcc -print-multi-os-directory); pwd)
}

args="--prefix=/usr --sysconfdir=/etc --libdir=$(libdir /usr/lib)"

echo
echo "----------------------------------------------------------------"
echo " init done -> run ./configure CFLAGS='-g -O2' $args             "
echo "----------------------------------------------------------------"
echo
