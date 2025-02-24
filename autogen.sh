#!/bin/sh

VERSION=$(git describe --tags --abbrev=0 2>/dev/null | sed 's/^v//')
if [ -z "$VERSION" ]; then
  VERSION="0.0.1-dev"
fi
sed -i "s/@VERSION@/$VERSION/" configure.ac

aclocal
autoconf
automake --add-missing
./configure
