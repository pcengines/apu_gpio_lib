#include <stdio.h>
#include <unistd.h>
#include <apu2_gpio.h>

int main(void) {
    int status, i;
    unsigned val = 1;

    printf("Library version: %s\n", apu_gpio_version());

    status = apu_gpio_init();
    if (status < 0) {
        printf("GPIO library init failed: %d\n", status);
        return -1;
    }

    //
    // set led gpios direction -> out
    //
    for (i = 0; i < 3; i++) {
        status = apu_gpio_set_dir(APU_LED1 + i, APU_DIR_OUT);
        if (status < 0) {
            printf("LED%d direction set failed: %d\n", i, status);
        }
    }

    while (1) {
        apu_gpio_set_val(APU_LED2, val);
        val ^= 1;
        apu_gpio_set_val(APU_LED3, val);
        sleep(1);
    }

    return 0;
}
