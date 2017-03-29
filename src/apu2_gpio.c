/**
 *
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <apu2_gpio.h>

#define FCH_ACPI_MMIO_BASE      0xFED80000
#define FCH_GPIO_BASE           (FCH_ACPI_MMIO_BASE + 0x1500)
#define FCH_GPIO_SIZE           0x300

volatile unsigned *gpio;

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
        NULL,                   // Any adddress in our space will do
        FCH_GPIO_SIZE,          // Map length
        PROT_READ|PROT_WRITE,   // Enable reading & writting to mapped memory
        MAP_SHARED,             // Shared with other processes
        mem_fd,                 // File to map
        FCH_GPIO_BASE           // Offset to GPIO peripheral
   );

   close(mem_fd); // No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      return APU_MAP_FAILED;
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;
}
