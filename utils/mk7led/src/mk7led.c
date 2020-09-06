/*
    This is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Kismet; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdint.h>

/*
 * All commands are 'MK7LED' followed by 4 bytes of command.
 *
 * MK7LED R 000                 Reset to defaults
 * MK7LED P S [0-255] 0         Set pulse speed, lower numbers are slower
 * MK7LED P 0 [0-255] 0         Set pulse state 0 LED hue
 * MK7LED P 1 [0-255] 0         Set pulse state 1 LED hue
 * MK7LED [0-3] [H] [S] [V]     Enter manual mode, set LED # HSV value
 */

void help(const char *argv0) {
    printf("mk7led control\n");
    printf("usage: %s [OPTIONS]\n", argv0);
    printf(" -r, --reset                 Reset LEDs to default\n"
           " -0, --led0 [H,S,V]          Set LED0 color to H, S, V\n"
           " -1, --led1 [H,S,V]          Set LED0 color to H, S, V\n"
           " -2, --led2 [H,S,V]          Set LED0 color to H, S, V\n"
           " -3, --led3 [H,S,V]          Set LED0 color to H, S, V\n"
           " -p, --pulse-speed [S,L]     Set pulse speed, lower is slower\n"
           " -a, --pulse-a [H,S]         Set pulse color A hue, saturation\n"
           " -b, --pulse-b [H,S]         Set pulse color B hue, saturation\n"
           "\n"
           "Pulse Speeds\n"
           " S, slow fractional range from 0-255 (default is 66)\n"
           " L, large range (default is 0)\n"
           "\n"
           "HSV Values\n"
           " Hue ranges from 0 to 360\n"
           " Saturation and Value range from 0 to 255\n"
           " HSV colors match the HSV color wheel:\n"
           "    http://colorizer.org/\n"
           );

}

uint8_t hsv_downsample(unsigned int s) {
    return (uint8_t) ((float) ((float) s / 360.0f) * 255.0f);
}

void send_cmd(int fd, uint8_t *cmd) {
    uint8_t header[] = {0, 0, 0, 0, 'M', 'K', '7', 'L', 'E', 'D'};

    write(fd, header, 10);
    write(fd, cmd, 4);
}

int main(int argc, char * const argv[]) {
    int fd;
    struct termios options;

    static struct option longopt[] = {
        {"reset", no_argument, 0, 'r'},

        {"led0", required_argument, 0, '0'},
        {"led1", required_argument, 0, '1'},
        {"led2", required_argument, 0, '2'},
        {"led3", required_argument, 0, '3'},

        {"pulse-speed", required_argument, 0, 'p'},
        {"pulse-a", required_argument, 0, 'a'},
        {"pulse-b", required_argument, 0, 'b'},

        {0, 0, 0, 0},
    };

    int option_idx = 0;
    optind = 0;
    opterr = 0;
    int r;

    bool reset = false;

    bool set0 = false;
    uint8_t h0, s0, v0;

    bool set1 = false;
    uint8_t h1, s1, v1;

    bool set2 = false;
    uint8_t h2, s2, v2;

    bool set3 = false;
    uint8_t h3, s3, v3;

    bool setpulsespeed = false;
    uint8_t pulsespeed;
    uint8_t pulsefspeed;

    bool setpulsea = false;
    uint8_t pulsea;
    uint8_t pulsesa;

    bool setpulseb = false;
    uint8_t pulseb;
    uint8_t pulsesb;

    unsigned int pv1, pv2, pv3;

    uint8_t serbuf[4] = {0, 0, 0, 0};

    while (1) {
        r = getopt_long(argc, argv, "-hr0:1:2:3:p:a:b:", longopt, &option_idx);

        if (r < 0)
            break;

        if (r == 'h') {
            help(argv[0]);
            exit(1);
        }

        if (r == 'r')
            reset = true;

        if (r == '0') {
            if (sscanf(optarg, "%u,%u,%u", &pv1, &pv2, &pv3) != 3) {
                fprintf(stderr, "ERROR:  Expected H,S,V\n");
                exit(1);
            }

            set0 = true;
            h0 = hsv_downsample(pv1);
            s0 = pv2;
            v0 = pv3;
        }

        if (r == '1') {
            if (sscanf(optarg, "%u,%u,%u", &pv1, &pv2, &pv3) != 3) {
                fprintf(stderr, "ERROR:  Expected H,S,V\n");
                exit(1);
            }

            set1 = true;
            h1 = hsv_downsample(pv1);
            s1 = pv2;
            v1 = pv3;
        }

        if (r == '2') {
            if (sscanf(optarg, "%u,%u,%u", &pv1, &pv2, &pv3) != 3) {
                fprintf(stderr, "ERROR:  Expected H,S,V\n");
                exit(1);
            }

            set2 = true;
            h2 = hsv_downsample(pv1);
            s2 = pv2;
            v2 = pv3;
        }

        if (r == '3') {
            if (sscanf(optarg, "%u,%u,%u", &pv1, &pv2, &pv3) != 3) {
                fprintf(stderr, "ERROR:  Expected H,S,V\n");
                exit(1);
            }

            set3 = true;
            h3 = hsv_downsample(pv1);
            s3 = pv2;
            v3 = pv3;
        }

        if (r == 'p') {
            pv1 = 0;
            pv2 = 0;

            if (sscanf(optarg, "%u,%u", &pv1, &pv2) != 2) {
                if (sscanf(optarg, "%u", &pv1) != 1) {
                    fprintf(stderr, "ERROR:  Expected 0-255 pulse speed\n");
                    exit(1);
                }
            }

            setpulsespeed = true;
            pulsespeed = pv1;
            pulsefspeed = pv2;
        }

        if (r == 'a') {
            if (sscanf(optarg, "%u,%u", &pv1, &pv2) != 2) {
                fprintf(stderr, "ERROR:  Expected 0-360 color hue\n");
                exit(1);
            }

            setpulsea = true;
            pulsea = pv1;
            pulsesa = pv2;
        }

        if (r == 'b') {
            if (sscanf(optarg, "%u,%u", &pv1, &pv2) != 2) {
                fprintf(stderr, "ERROR:  Expected 0-360 color hue\n");
                exit(1);
            }

            setpulseb = true;
            pulseb = pv1;
            pulsesb = pv2;
        }
    }

    if (!reset && 
        !set0 && !set1 && !set2 && !set3 &&
        !setpulsespeed && !setpulsea && !setpulseb) {
        fprintf(stderr, "Nothing to do...\n");
        help(argv[0]);
        exit(1);
    }

    fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY);

    if (fd < 0) {
        fprintf(stderr, "ERROR:  Failed to open /dev/ttys0: %s\n", strerror(errno));
        exit(1);
    }

    tcgetattr(fd, &options);

    options.c_oflag = 0;
    options.c_iflag = 0;

    options.c_iflag &= (IXON | IXOFF | IXANY);
    options.c_cflag |= CLOCAL | CREAD;
    options.c_cflag &= ~HUPCL;

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    if (tcsetattr(fd, TCSANOW, &options) < 0) {
        fprintf(stderr, "ERROR:  Failed to configure /dev/ttyS0: %s\n", strerror(errno));
        exit(1);
    }
    
    if (reset) {
        serbuf[0] = 'R';
        send_cmd(fd, serbuf);
    }

    if (set0) {
        serbuf[0] = '0';
        serbuf[1] = h0;
        serbuf[2] = s0;
        serbuf[3] = v0;
        send_cmd(fd, serbuf);
    }

    if (set1) {
        serbuf[0] = '1';
        serbuf[1] = h1;
        serbuf[2] = s1;
        serbuf[3] = v1;
        send_cmd(fd, serbuf);
    }

    if (set2) {
        serbuf[0] = '2';
        serbuf[1] = h2;
        serbuf[2] = s2;
        serbuf[3] = v2;
        send_cmd(fd, serbuf);
    }

    if (set3) {
        serbuf[0] = '3';
        serbuf[1] = h3;
        serbuf[2] = s3;
        serbuf[3] = v3;
        send_cmd(fd, serbuf);
    }

    if (setpulsespeed) {
        serbuf[0] = 'P';
        serbuf[1] = 'S';
        serbuf[2] = pulsespeed;
        serbuf[3] = pulsefspeed;
        send_cmd(fd, serbuf);
    }

    if (setpulsea) {
        serbuf[0] = 'P';
        serbuf[1] = '0';
        serbuf[2] = pulsea;
        serbuf[3] = pulsesa;
        send_cmd(fd, serbuf);
    }

    if (setpulseb) {
        serbuf[0] = 'P';
        serbuf[1] = '1';
        serbuf[2] = pulseb;
        serbuf[3] = pulsesb;
        send_cmd(fd, serbuf);
    }

    close(fd);
    return 0;
}

