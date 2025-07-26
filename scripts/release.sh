#/bin/bash

PROG=proj
VER=`date +%Y%m%d-%H%M`
DIR="${1:-build}"

tar acfvp $PROG-$VER.tar.gz  ${DIR}/bin ${DIR}/doc ${DIR}/etc