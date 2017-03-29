/**
 * APU2/3 GPIO access library
 */

#ifndef APU2_GPIO_H
#define APU2_GPIO_H

//
// Error codes returned by library
//
#define APU_SUCCESS      0  // success
#define APU_IO_ERROR    -1  // i/o error opening mem file
#define APU_MAP_FAILED  -2  // mem mapping failed
#define APU_INV_PARAM   -3  // invalid parameter
#define APU_NOT_INIT    -4  // library not initialized

//
// GPIO offsets definitions
//
#define APU_GPIO_57     0x44
#define APU_GPIO_58     0x45
#define APU_GPIO_59     0x46
#define APU_GPIO_32     0x59
#define APU_GPIO_33     0x5A
#define APU_LED1        APU_GPIO_57
#define APU_LED2        APU_GPIO_58
#define APU_LED3        APU_GPIO_59
#define APU_MODESW      APU_GPIO_32
#define APU_SIMSWAP     APU_GPIO_33

//
// GPIO states
//
#define APU_DIR_IN      0x0
#define APU_DIR_OUT     0x1

int apu_gpio_init(void);
int apu_gpio_get_dir(unsigned offset);
int apu_gpio_set_dir(unsigned offset, unsigned direction);
int apu_gpio_get_val(unsigned offset);
int apu_gpio_set_val(unsigned offset, unsigned value);

#endif
