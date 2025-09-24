#/bin/bash

PROG=proj
VERSION=`grep -Po 'project\([^)]*VERSION\s+\K[\d.]+' CMakeLists.txt`
TS=`date +%Y%m%d.%H%M`
DIR=cpp-skeleton

FILE=$PROG-v$VERSION-t$TS.zip

cd ..
zip -r $FILE $DIR --exclude \
    "$DIR/.vscode" "$DIR/.vscode/*" \
    "$DIR/.git" "$DIR/.git/*" \
    "$DIR/.git" "$DIR/.git/*" \
    "$DIR/build*/" "$DIR/build*/*" \
    "$DIR/archive" "$DIR/archive/*"

mv $FILE $DIR/archive

echo "Archive project ($FILE) completed."