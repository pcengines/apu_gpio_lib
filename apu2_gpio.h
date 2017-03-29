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

/*
 * Returns library version string.
 *
 * @returns Version string
 */
const char *apu_gpio_version(void);

/*
 * Initializes the library
 *
 * @retval APU_SUCCESS      Lib init ok
 * @retval APU_IO_ERROR     Error opening /dev/mem file
 * @retval APU_MAP_FAILED   Error mapping the GPIO space
 */
int apu_gpio_init(void);

/*
 * Gets GPIO pin direction
 *
 * @param[in] offset        GPIO offset
 *
 * @returns                 1 - output, 0 - input
 * @retval APU_INV_PARAM    GPIO offset invalid
 * @retval APU_NOT_INIT     Library not initialized
 */
int apu_gpio_get_dir(unsigned offset);

/*
 * Sets GPIO pin direction
 *
 * @param[in] offset        GPIO offset
 * @param[in] direction     0 - input, other - output
 *
 * @retval APU_INV_PARAM    GPIO offset invalid
 * @retval APU_NOT_INIT     Library not initialized
 */
int apu_gpio_set_dir(unsigned offset, unsigned direction);

/*
 * Gets GPIO pin state
 *
 * @param[in] offset        GPIO offset
 *
 * @returns                 0 - low state, 1 - high state
 * @retval APU_INV_PARAM    GPIO offset invalid
 * @retval APU_NOT_INIT     Library not initialized
 */
int apu_gpio_get_val(unsigned offset);

/*
 * Sets GPIO pin value
 *
 * @param[in] offset        GPIO offset
 * @param[in] value         GPIO value
 *
 * @retval APU_INV_PARAM    GPIO offset invalid
 * @retval APU_NOT_INIT     Library not initialized
 */
int apu_gpio_set_val(unsigned offset, unsigned value);

#endif
