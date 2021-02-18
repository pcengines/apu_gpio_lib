/* **********************************************************************
**
** Copyright (c) 2021 Raimo Niskanen <raimo (at) niskanen (dot) nu>
**
** Permission to use, copy, modify, and distribute this software for any
** purpose with or without fee is hereby granted, provided that the above
** copyright notice and this permission notice appear in all copies.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
** WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
** ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
** WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
** ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
** **********************************************************************
**
** apu2_gpiod - a daemon to control the APU2/3 LEDs on OpenBSD
**
** WARNING: Do not use this code on any other hardware than
** PC Engines APU2 (tested) or APU3 (should work).
** The direct memory accesses may do anything including damage
** to other hardware.
**
** See apu_gpio_lib from PC Engines.  This program is built
** as one of its examples, and can be copied to for example
** /root/bin/.
**
** Start the daemon from rc.securelevel while direct memory
** access is allowed.
**
** Example rc.securelevel line:
**     /root/bin/apu2_gpiod /var/apu2_gpio
**
** The FIFO /var/apu2_gpio has to be created:
** # mkfifo -m 600 /var/apu2_gpio
**
** Then, after reboot, the LEDS can be controlled by writing
** sequences of 1-character LED commands to the FIFO.
** The LEDs are set to ON,OFF,OFF from left to right
** when the daemon starts.
**
** LED commands:
**   A,B,C  Set left,middle,right LED to ON
**   a,b,c  Set left,middle,right LED to OFF
**   1,2,3  Toggle left,middle,right LED
**   All other characters are ignored
**
** Examples:
** # echo abC > /var/apu/gpio	# Set only the right LED to ON 
** # echo 2: > /var/apu/gpio	# Toggle the middle LED
**
** The `:' in the latest example is used to avoid that the number
** `2' is interpreted as a file descriptor for the pipe operator
** and it is ignored as an unknown LED command.
**
** NOTE
** ====
**   The daemon forks into a led controller and a supervisor.
** If the led controller should fail e.g due to not being able
** to open the FIFO, the supervisor
** 1) sets the LEDs to OFF,ON,OFF, and
**    writes an entry to the syslog,
** 2) waits 60 s
** 3) set the LEDS to OFF,ON,ON, writes another entry
**    to the syslog, and exec:s "shutdown -r +1" to reboot
**    the system, hoping it will work better after reboot.
**
**   This behaviour may cause cyclic reboots.
** To get out of a cyclic reboot sequence, during the first 60 s
** this daemon may be killed, and during the last 60 s
** the shutdown process may be killed.
**   The idea with all this is that the only way to restart
** the led controller is during reboot from rc.securelevel,
** and that the LEDs are essential to the system.  If the
** behaviour is not what you want, go ahead and change the code...
**
**   There are remnants in the code of reading the MODESW input
** but that track was abandoned.
**
** **********************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/wait.h>

#include <apu2_gpio.h>

int led_handler(char *fname) {
    int sw, sw_prev;

    apu_gpio_set_val(APU_LED1, 0);
    apu_gpio_set_val(APU_LED2, 1);
    apu_gpio_set_val(APU_LED3, 0);

    sw = apu_gpio_get_val(APU_MODESW);
    sw_prev = sw;
    syslog(LOG_DAEMON | LOG_ERR, "MODESW start value %d", sw);

    for (;;) {
	int fifo;
	struct stat sb;

	fifo = open(fname, O_RDONLY);
	if (fifo < 0) {
	    syslog(LOG_DAEMON | LOG_ERR, "Could not open fifo: %m");
	    return 3;
	}
	if (fstat(fifo, &sb) < 0) {
	    syslog(LOG_DAEMON | LOG_ERR, "Could not fstat fifo: %m");
	    return 4;
	}
	if (! S_ISFIFO(sb.st_mode)) {
	    syslog(LOG_DAEMON | LOG_ERR, "Fifo is not a fifo");
	    return 5;
	}

	for (;;) {
	    ssize_t n;
	    char c;

	    n = read(fifo, &c, 1);
	    if (n == 0) {
		if (close(fifo) < 0) {
		    syslog(LOG_DAEMON | LOG_ERR, "Close failed after EOF from fifo: %m");
		}
		break;
	    }
	    if (n < 0) {
		syslog(LOG_DAEMON | LOG_ERR, "Read error from fifo: %m");
		break;
	    }

	    sw = apu_gpio_get_val(APU_MODESW);
	    if (sw != sw_prev) {
		syslog(LOG_DAEMON | LOG_ERR, "MODESW changed to %d", sw);
		sw_prev = sw;
	    }

	    if ('A' <= c && c <= 'C') {
		/* LED on (active low) */
		apu_gpio_set_val(APU_LED1 + (c - 'A'), 0);
	    }
	    else if('a' <= c && c <= 'c') {
		/* LED off (active low) */
		apu_gpio_set_val(APU_LED1 + (c - 'a'), 1);
	    }
	    else if('1' <= c && c <= '3') {
		/* LED off (active low) */
		apu_gpio_set_val(
		    APU_LED1 + (c - '1'),
		    apu_gpio_get_val(APU_LED1 + (c - '1')) ^ 1);
	    }
	}
    }
}

int shutdown_watcher(int child) {
    int status, sw;
    unsigned int t, i;
    char *const shutdown[] =
	{"/sbin/shutdown", "-r", "+1", "apu_gpio LED handler failed", NULL};

    (void) waitpid(child, &status, 0);
    syslog(
	LOG_DAEMON | LOG_ERR,
	"apu_gpio LED handler exited: %d, shutdown in 1.5 min", status);
    apu_gpio_set_val(APU_LED1, 1);
    apu_gpio_set_val(APU_LED2, 0);
    apu_gpio_set_val(APU_LED3, 1);

    /* Wait in case of direct fail from rc.securelevel, no rush in other cases */
    for (t = 60;  t != 0;  t = sleep(t));

    sync();
    sync();
    apu_gpio_set_val(APU_LED3, 0);
    /* Now hopefully the system is up and can be brought down in good order */
    (void) execv(shutdown[0], shutdown);
    syslog(LOG_DAEMON | LOG_ERR, "Call to shutdown failed: %m");

    return 2;
}

int main(int argc, char *argv[]) {
    pid_t child;
    int status, i;

    if (argc < 2) {
	fprintf(stderr, "Missing argument: LED fifo name\n");
	exit(1);
    }

    printf("starting apu2_gpiod with library version: %s\n", apu_gpio_version());
    status = apu_gpio_init();
    if (status < 0) {
	fprintf(stderr, "apu_gpio library init failed: %d\n", status);
	exit(1);
    }
    for (i = 0; i < 3; i++) {
	status = apu_gpio_set_dir(APU_LED1 + i, APU_DIR_OUT);
	if (status < 0) {
	    fprintf(stderr, "apu_gpio LED%d direction set failed: %d\n", i, status);
	    exit(1);
	}
    }
    status = apu_gpio_set_dir(APU_MODESW, APU_DIR_IN);
    if (status < 0) {
	fprintf(stderr, "apu_gpio SW direction set failed: %d\n", status);
	exit(1);
    }

    if (daemon(0, 0) != 0) {
	perror("Could not daemonize");
	exit(1);
    }

    child = fork();
    if (child == -1) {
	syslog(LOG_DAEMON | LOG_ERR, "Could not fork led handler - %m");
	exit(1);
    }
    if (child == 0) {
	_exit(led_handler(argv[1]));
    }
    else {
	exit(shutdown_watcher(child));
    }
}
