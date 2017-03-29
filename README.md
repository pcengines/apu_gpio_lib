APU2/3 GPIO access library
==========================

This is library for accessing the GPIO's of APU2/3 devices. Currently it
supports accessing the GPIO's controlling the LED1,2,3, MODESW signal and
SIMSWAP signal (apu3 only).

This library could be used as an example.

Required tools to build
-----------------------

* gcc
* cmake

How to compile
--------------

By default compiles shared library and example blinky program.

```
mkdir build
cd build
cmake ..
make
```
