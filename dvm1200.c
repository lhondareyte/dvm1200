/*
 * Copyright (c) 2017-2024 Luc Hondareyte
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h> 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <unistd.h>

#ifndef clear_bit
 #define clear_bit(value,bit)	( value &= ~(1<<bit))
#endif

#ifndef bit_is_set
 #define bit_is_set(var,pos) ((var) & (1<<(pos)))
#endif

extern char d_decode(uint8_t);
extern void u_decode(uint8_t, uint8_t, uint8_t);
void get_time(void);

#if defined (__APPLE__)
#define 	DEFAULT_PORT	"/dev/cu.usbserial"
#elif defined (__FreeBSD__)
#define 	DEFAULT_PORT	"/dev/cuaU0"
#elif defined (__linux__)
#define 	DEFAULT_PORT	"/dev/ttyUSB0"
#elif defined (__NetBSD__)
#define 	DEFAULT_PORT	"/dev/ttyU0"
#elif defined (__OpenBSD__)
#define 	DEFAULT_PORT	"/dev/cuaU0"
#endif

struct timeval  tv;
struct tm       *tm;
struct termios tty;
char *device = DEFAULT_PORT;
char t_fmt[64], t_buf[64];
int	count=0;
int	fd=0;

static void usage(void) {
	fprintf (stderr, "Usage : dvm1200 [-d device] [-c count] [-u]\n");
	exit (EXIT_FAILURE);
}

inline void get_time(void) 
{
	gettimeofday(&tv, NULL);
	if((tm = localtime(&tv.tv_sec)) != NULL) {
		strftime(t_fmt, sizeof t_fmt, "%Y-%m-%d , %H:%M:%S ,", tm);
		snprintf(t_buf, sizeof t_buf, t_fmt, tv.tv_usec);
	}
}

int set_interface_attribs(int fd, int speed)
{
	if (tcgetattr(fd, &tty) < 0) {
		fprintf(stderr, "Error : %s : %s.\n", device, strerror(errno));
		return -1;
	}

	cfsetospeed(&tty, (speed_t)speed);
	cfsetispeed(&tty, (speed_t)speed);

	tty.c_cflag &= ~(PARENB|CSTOPB|CRTSCTS);
	tty.c_cflag |=  (CSIZE|CS8|CLOCAL|CREAD );

	/* non-canonical mode */
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	tty.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	tty.c_oflag &= ~OPOST;

	/* One byte at a time on startup */
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 1;

	if (tcsetattr(fd, TCSANOW, &tty) < 0) {
		fprintf(stderr, "Error : %s : %s.\n", device, strerror(errno));
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int i=0;                /* loop variable */
	int maxcount = 0;       /* max read before exit */
	bool uniq = false;      /* -u option (uniq) */
	int opt;                /* store arg */
	int rc = EXIT_SUCCESS;  /* return code */
	int rdlen;	            /* read lenght */
	uint8_t  buf[80];       /* buffer */
	uint8_t p_buf[80];      /* previous buffer */
	uint8_t c=0;            /* store received byte */
	uint8_t j=0;            /* store received byte to check bytes sequence */

	opterr = 0;             /* disable getopt() abort message */
	
	/* Checking command line options */
	if (argc > 1) {
		/* -d device -c maxcount */
		while ((opt = getopt (argc, argv, "d:c:u")) != -1 ) {
			switch (opt) {
			case 'd': 
				device = optarg; 
				break;
			case 'c': 
				if (! isdigit(*optarg)) {
					fprintf (stderr ,"Error : not a number (%s).\n", optarg);
					exit (EXIT_FAILURE);
				}
				maxcount = atoi(optarg); 
				break;
			case 'u':
				uniq = true;
				break;
			default:
				usage();
				break;
			}
		}
	}
	if (optind < argc) {
		usage();
	}

	/* Unbuffered for stdout */
	setvbuf(stdout, (char*)NULL, _IONBF, 0);

Restart:
	fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		fprintf (stderr, "Error : %s : %s.\n", device, strerror(errno));
		exit (EXIT_FAILURE);
	}

	/* baudrate 2400 */
	if (set_interface_attribs(fd, B2400) < 0) {
		rc = EXIT_FAILURE;
		goto End;
	}

	/* Waiting for 0xf1 : the last byte from last packet */
	rdlen=0;

	fprintf (stderr, "Waiting the end of last packet... ");
	while(1) {
		rdlen = read(fd, buf, sizeof(buf) - 1);
		if (rdlen == 1 && buf[0] == 0xf1) {
			fprintf(stderr, "ok\n");
			break;
		}
	}

	/* Now, we get 15 characters at a time */
	tty.c_cc[VMIN] = 15;
	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		fprintf (stderr, "Error : %s : %s.\n", device, strerror(errno));
		rc = EXIT_FAILURE;
		goto End;
	}
	memset(buf, 0, sizeof buf);
	memset(p_buf, 0, sizeof p_buf);

	/* Main loop */
	while(1) {
		if ((rdlen = read(fd, buf, sizeof(buf) - 1)) > 0 ) {
			if ((memcmp(p_buf, buf, rdlen) == 0) && ( uniq == true )) {
				continue;
			}
			else {
				get_time();
				count++;
				memcpy(p_buf, buf, rdlen);
#ifndef __DEBUG_DVM__
				/* Print sequence number and time for each line */
				fprintf(stdout, "%05d , %s ", count, t_buf);
#endif
				/* Check buffer consistency and remove MSB */
				for (i = 0; i< rdlen; i++) {
					j = buf[i]; 
					j = j>>4;
					if (j != i+1) {
						fprintf(stderr, "\nSequence error in buffer!\n");
						close(fd);
					    	goto Restart;
					}
					buf[i] &= 0x0f;
				}
				/* Decoding numbers */
#if defined (__DEBUG_DVM__)
				for (i = 1; i < 15; i += 2) {
					c=(buf[i]<<4)+(buf[i+1]);
					fprintf(stdout, " %x", c);
				}
				fprintf(stdout, " : ");
#endif
				for (i = 1; i < 9; i += 2) {
					c=(buf[i]<<4)+(buf[i+1]);
					/* Checking for dot or minus sign */
					if (bit_is_set(c,4)) {
						clear_bit(c,4);
						/* Minus sign at the left */
						if (i < 3)
							fprintf(stdout, "-%c",d_decode(c));
						/* Dot sign at the left */ 
						else fprintf(stdout, ".%c",d_decode(c));
					} else {
						fprintf(stdout, "%c",d_decode(c));
					}
				}
				fprintf(stdout, " , ");
				/* Decoding scales and units */
				u_decode( (buf[9]<<4)+(buf[10]), (buf[11]<<4)+(buf[12]), (buf[13]<<4)+(buf[14]) );
				fprintf(stdout, " , \n");
				if ((maxcount != 0) && (maxcount >= count)) {
					break;
				}
			}
		} else {
			fprintf(stderr, "Error reading device %s!\n", device);
			rc = EXIT_FAILURE;
			break;
		}
	}

End:
	close(fd);
	exit(rc);
}
