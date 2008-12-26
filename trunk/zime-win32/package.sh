#!/bin/bash

PKG=combo-pinyin
VER=1.2
SRC=`pwd`
TEMP=tmp-dir
DEST=$TEMP/$PKG

mkdir -p $DEST
cp $SRC/combo-pinyin.exe $DEST
cp $SRC/*.txt $DEST
mkdir $DEST/config
cp $SRC/config/*.txt $DEST/config
mkdir $DEST/doc
cp $SRC/doc/*.pdf $SRC/doc/*.png $SRC/doc/*.txt $DEST/doc
cp -R $SRC/Microsoft.VC80.CRT $DEST

cd $TEMP
zip -r $SRC/$PKG-$VER.zip $PKG
cd ..
rm -Rf $TEMP
