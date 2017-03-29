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

Example
-------

After compilation. Example is in dir `build/example/blinky`.
It blinks the LED2 and LED3 on APU.

Library API
-----------

* `int apu_gpio_init(void)` - initializes the library and maps the gpio space
* `int apu_gpio_get_dir(unsigned offset)` - gets gpio direction (1 - out, 0 - in)
* `int apu_gpio_set_dir(unsigned offset, unsigned direction)` - sets gpio direction (1 - out, 0 - in)
* `int apu_gpio_get_val(unsigned offset)` - gets gpio state
* `int apu_gpio_set_val(unsigned offset, unsigned value)` - sets gpio value
