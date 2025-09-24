#/bin/bash

PROG=proj
VERSION=`grep -Po 'project\([^)]*VERSION\s+\K[\d.]+' CMakeLists.txt`
TS=`date +%Y%m%d-%H%M`
DIR="${1:-build}"

tar acfvp $PROG-v$VERSION.tar.gz --transform "s/^/$PROG\//" -C ${DIR} --ignore-failed-read bin doc etc include lib
mv $PROG-v$VERSION.tar.gz archive/

echo "Release project ($PROG-v$VERSION.tar.gz) completed."