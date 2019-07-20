#!/bin/sh

osname=`uname` # Linux/Darwin
echo "osname: $osname"

mkdir -p cbuild
cd cbuild

if [ $osname == "Linux" ]; then
    cmake .. -DBUILD_SERVER=ON -DBUILD_EDGE=OFF -DBUILD_CLIENT=OFF -DCMAKE_BUILD_TYPE=Debug
elif [ $osname == "Darwin" ]; then
    cmake .. -DBUILD_SERVER=ON -DBUILD_EDGE=OFF -DBUILD_CLIENT=OFF -DCMAKE_BUILD_TYPE=Debug
fi

make -j8
