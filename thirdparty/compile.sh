#!/bin/bash
export WD=$(pwd)
#HIDAPI
echo "Compiling HIDAPI..."
cd hidapi
./bootstrap > /dev/null
./configure --prefix=$WD/../install --exec-prefix=$WD/../install --enable-static --disable-shared > /dev/null
make > /dev/null
make install > /dev/null
#Compile libtempered
echo "Compiling libtempered..."
cd ../libtempered
cmake -DHIDAPI_LIB=$WD/install/lib/libhidapi-libusb.a -DCMAKE_INSTALL_PREFIX:PATH=$WD/install/ .
make > /dev/null
make install