/**
 *
 */

#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <apu2_gpio.h>

#define FCH_ACPI_MMIO_BASE      0xFED80000
#define FCH_GPIO_OFFSET         0x1500
#define FCH_GPIO_SIZE           0x300

#define GPIO_BIT_DIR            23
#define GPIO_BIT_WRITE          22
#define GPIO_BIT_READ           16

static volatile unsigned *gpio;
static unsigned initialized = 0;

int apu_gpio_init(void) {

    int mem_fd;
    void *gpio_map;

    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        return APU_IO_ERROR;
    }

    //
    // mmap the GPIO space
    //
    gpio_map = mmap(
        NULL,                               // Any adddress in our space will do
        FCH_GPIO_OFFSET + FCH_GPIO_SIZE,    // Map length
        PROT_READ|PROT_WRITE,               // Enable reading & writting to mapped memory
        MAP_SHARED,                         // Shared with other processes
        mem_fd,                             // File to map
        FCH_ACPI_MMIO_BASE                  // Offset of the MMIO BASE
   );

   close(mem_fd); // No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      return APU_MAP_FAILED;
   }

   gpio = (volatile unsigned *)(gpio_map + FCH_GPIO_OFFSET);

   initialized = 1;

   return APU_SUCCESS;
}

static unsigned is_offset_valid(unsigned offset) {
    if (offset > FCH_GPIO_SIZE) {
        return 0;
    }
    return 1;
}

int apu_gpio_get_dir(unsigned offset)
{
    uint32_t val;

    if (!initialized)
        return APU_NOT_INIT;

    if (!is_offset_valid(offset))
        return APU_INV_PARAM;

    val = *(gpio + offset);
    return (val >> GPIO_BIT_DIR) & 1;
}

int apu_gpio_set_dir(unsigned offset, unsigned direction)
{
    volatile uint32_t *val;

    if (!initialized)
        return APU_NOT_INIT;

    if (!is_offset_valid(offset))
        return APU_INV_PARAM;

    val = gpio + offset;
    if (direction == APU_DIR_IN)
        *val &= ~(1 << GPIO_BIT_DIR);
    else
        *val |= (1 << GPIO_BIT_DIR);

    return APU_SUCCESS;
}

int apu_gpio_get_val(unsigned offset)
{
    uint32_t val;

    if (!initialized)
        return APU_NOT_INIT;

    if (!is_offset_valid(offset))
        return APU_INV_PARAM;

    val = *(gpio + offset);
    return (val >> GPIO_BIT_READ) & 1;
}

int apu_gpio_set_val(unsigned offset, unsigned value)
{
    volatile uint32_t *val;

    if (!initialized)
        return APU_NOT_INIT;

    if (!is_offset_valid(offset))
        return APU_INV_PARAM;

    val = gpio + offset;

    if (value)
        *val |= (1 << GPIO_BIT_WRITE);
    else
        *val &= ~(1 << GPIO_BIT_WRITE);

    return APU_SUCCESS;
}
