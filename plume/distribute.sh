#!/bin/bash
if [ $# != 1 ]
    then
    echo "version number not provided."
    exit -1
fi
ver=$1
target=plume-${ver}
if [ -e $target ]
    then
    echo "error: $target already exists."
    exit -1
fi
svn export . temp || exit -1
cd temp
ant build || exit -1
rm -R war/WEB-INF war/test.html war/script/test.js war/script/sample.json
mkdir war/data
cp data/make-json.py war/data/
cp LICENSE.txt README.txt war/
mv war $target
zip -r ../$target.zip $target
cd ..
rm -R temp
echo "done."
