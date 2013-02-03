node-tempered
=============

A Node.JS API for the TEMPered library from https://github.com/edorfaus/TEMPered

h2. Installation

* Compile and install HIDAPI as described on the TEMPered page
* Compile and install libtempered. libtempered-util is not needed by this software
* This program should work with a static libtempered library, however I didn't test that.
* Compile node-tempered
```
npm install
```
* If compilation fails, you probably don't have libtempered installed correctly.

Maybe I'll find the time to include the libtempered and hidapi source code in node-tempered to ensure better compatibility, but right now you need to download and compile it manually!
