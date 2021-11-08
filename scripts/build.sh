#!/bin/sh
set -e
. ./config.sh

mkdir -p "$SYSROOT"

# Copy the headers first.
for PROJECT in $SYSTEM_HEADER_PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install-headers)
done

for PROJECT in $PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE install)
done
