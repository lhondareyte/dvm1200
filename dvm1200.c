/*
 * Copyright (c)2017,  Luc Hondareyte <lhondareyte_AT_laposte.net>.
 * All rights reserved.
 *   
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in 
 *    the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 *
 */

#include <errno.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>

#ifndef clear_bit
 #define clear_bit(value,bit)	( value &= ~(1<<bit))
#endif

#ifndef bit_is_set
 #define bit_is_set(var,pos) ((var) & (1<<(pos)))
#endif

extern char d_decode(uint8_t);
extern void u_decode(uint8_t, uint8_t, uint8_t);

#if defined (__FreeBSD__)
#define 	DEFAULT_PORT	"/dev/cuaU0"
#endif

#if defined (__NetBSD__)
#define 	DEFAULT_PORT	"/dev/ttyU0"
#endif

#if defined (Linux)
#define 	DEFAULT_PORT	"/dev/ttyUSB0"
#endif

#if defined (__APPLE__)
#define 	DEFAULT_PORT	"/dev/usbserial1"
#endif

#if defined (__REPLEO_DRIVER__)
#undef 		DEFAULT_PORT
#define 	DEFAULT_PORT	"/dev/cu.Repleo-PL2303-00008114"
#endif

#define MASK 0xf0

#include <sys/time.h>
char            t_fmt[64], t_buf[64];
struct timeval  tv;
struct tm       *tm;


struct	termios tty;
int	count=0;
int	fd=0;

void get_time(void) {
	gettimeofday(&tv, NULL);
	if((tm = localtime(&tv.tv_sec)) != NULL)
	{
		strftime(t_fmt, sizeof t_fmt, "%Y-%m-%d , %H:%M:%S ,", tm);
		snprintf(t_buf, sizeof t_buf, t_fmt, tv.tv_usec);
	}
}

int set_interface_attribs(int fd, int speed)
{
	if (tcgetattr(fd, &tty) < 0) {
		fprintf(stderr, "Error from tcgetattr: %s\n", strerror(errno));
		return -1;
	}

	cfsetospeed(&tty, (speed_t)speed);
	cfsetispeed(&tty, (speed_t)speed);

	tty.c_cflag &= ~( PARENB | CSTOPB | CRTSCTS );
	tty.c_cflag |=  ( CSIZE | CS8 | CLOCAL | CREAD );

	/* non-canonical mode */
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

	/* One byte at a time on startup */
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 1;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		fprintf(stderr, "Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}


int main(int argc, char *argv[])
{
	int i=0;			// loop variable

	// For parsing command line
	char *device = DEFAULT_PORT;
	int interval = 0;
	int limit = 0;
	int rflag = 0;
	int c;
	opterr = 0;


	// Checking command line options
	if ( argc > 1 ) {
		/*
		   -d device
		   -i interval
		   -r (relative time)
		   -c count limit
		 */
		while (( c = getopt (argc, argv, "d:i:ru:v" )) != -1 )
			switch (c) {
				case 'v':
					fprintf ( stderr, "This option should print version number\n");
					return 0;
				case 'd': 
					device = optarg; 
					break;
				case 'i': 
					interval = atoi(optarg); 
					if ( ! isnumber(interval) ) {
						fprintf (stderr ,"Error : %s is not a number.\n", optarg);
						return -1;
					}
					break;
				case 'r': 
					rflag = 1; 
					break;
				case 'c': 
					limit = atoi(optarg); 
					if ( ! isnumber(limit) ) {
						fprintf (stderr ,"Error : %s is not a number.\n", optarg);
						return -1;
					}
					break;
				case '?':
					fprintf ( stderr, "Usage : %s [-d device] [-i interval] [-c count] [-r ]\n", argv[0]);
					return 1;
				default:
					abort();
			}
	
	}
	
	int rdlen=0;			
	uint8_t  buf[80];
	uint8_t p_buf[80];

	fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		fprintf (stderr, "Error opening %s: %s\n", device, strerror(errno));
		return -1;
	}

	/*baudrate 2400, 8 bits, no parity, 1 stop bit */
	set_interface_attribs(fd, B2400);

	/* Unbuffered for stdout */
	setvbuf(stdout, (char*)NULL, _IONBF, 0);

	/* Waiting for 0xf1 : the last byte from last packet */
	fprintf (stderr, "Waiting the end of last packet... ");
	while(1)
	{
		rdlen = read(fd, buf, sizeof(buf) - 1);
		if ( rdlen == 1 && buf[0] == 0xf1 ) {
			fprintf(stderr, "ok\n");
			break;
		}
	}

	/* Now, we get 15 characters at a time */
	tty.c_cc[VMIN] = 15;
	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		fprintf (stderr, "Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	memset ( buf, 0, sizeof buf);
	memset ( p_buf, 0, sizeof p_buf);

	/* Main loop */
	while(1)
	{
		rdlen = read(fd, buf, sizeof(buf) - 1);
		if (rdlen > 0) {
			if (memcmp(p_buf, buf, rdlen) != 0)
			{
				get_time();
				count++;
				memcpy(p_buf, buf, rdlen);
#ifndef __DEBUG_DVM__
				// Print sequence number and time for each line
				fprintf(stdout, "%05d , %s ", count, t_buf);
#endif
				// Check buffer consistency and remove MSB
				for ( i=0; i< rdlen; i++) {
					uint8_t j=0;
					j = buf[i]; 
					j = j>>4;
					if ( j != i+1 ) {
						fprintf(stdout, "\nSequence error in buffer!!\n");
					       	exit(-1);
					}
					buf[i] &= 0x0f;
				}
				// Decode numbers
				uint8_t c=0;
#if defined (__DEBUG_DVM__)
				// Debug dvm1200 output
				for ( i=1; i<15; i+=2 ) {
					c=(buf[i]<<4)+(buf[i+1]);
					fprintf(stdout, " %x", c);
				}
				fprintf(stdout, " : ");
#endif
				for ( i=1; i<9; i+=2 ) {
					c=(buf[i]<<4)+(buf[i+1]);
					// Checking for dot or minus sign
					if (bit_is_set(c,4)) {
						clear_bit(c,4);
						// Minus sign at the left
						if (i < 3 )
							fprintf(stdout, "-%c",d_decode(c));
						// Dot sign at the left
						else fprintf(stdout, ".%c",d_decode(c));
					} else {
						fprintf(stdout, "%c",d_decode(c));
					}
				}
				fprintf(stdout, " , ");
				// Decode scaling and units
				u_decode( (buf[9]<<4)+(buf[10]), (buf[11]<<4)+(buf[12]), (buf[13]<<4)+(buf[14]) );
				fprintf(stdout, " , \n");
			}
		} else {
			fprintf(stderr, "Error from read: %d: %s\n", rdlen, strerror(errno));
			close(fd);
			exit (-1);
		}
	}
	close(fd);
	return 0;
}

