#include <stdio.h>
#include <stdlib.h>
#include <apu2_gpio.h>

void usage(char *app_name) {
    printf("Usage:\n\t%s value\n", app_name);
}

int main(int argc, char *argv[]) {

    int status;
    int val;

    if (argc < 2) {
        usage(argv[0]);
        return -1;
    }

    switch (argv[1][0]) {
        case '1':
            val = 1;
            break;
        case '0':
            val = 0;
            break;
        default:
            usage(argv[0]);
            return -1;
    }

    status = apu_gpio_init();
    if (status < 0) {
        printf("can't initialize the gpio lib: %d\n", status);
        return -1;
    }

    status = apu_gpio_set_dir(APU_SIMSWAP, APU_DIR_OUT);
    if (status < 0) {
        printf("can't set the simswap direction: %d\n", status);
        return -1;
    }

    status = apu_gpio_set_val(APU_SIMSWAP, val);
    if (status < 0) {
        printf("can't set the simswap value: %d\n", status);
    }

    return 0;
}
