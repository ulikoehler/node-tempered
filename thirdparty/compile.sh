#!/bin/bash
export WD=$(pwd)
#Note: We have to compile the static libraries with -fPIC because NodeJS links them into a shared library
#HIDAPI
echo "Compiling HIDAPI..."
cd hidapi
./bootstrap > /dev/null
./configure CFLAGS=-fPIC CPPFLAGS=-fPIC --prefix=$WD/install --exec-prefix=$WD/install --enable-static --disable-shared > /dev/null
make
make install > /dev/null
#Compile libtempered
echo "Compiling libtempered..."
cd ../tempered
cmake -DCMAKE_C_FLAGS=-fPIC -DBUILD_HIDAPI_SHARED=OFF -DHIDAPI_LINK_LIBS=udev -DBUILD_SHARED_LIB=OFF -DBUILD_STATIC_LIB=ON -DHIDAPI_LIB=$WD/install/lib/libhidapi-hidraw.a -DCMAKE_INSTALL_PREFIX:PATH=$WD/install/ .
make > /dev/null
make install