/*
 * Copyright (c)2016,  Luc Hondareyte <lhondareyte_AT_laposte.net>.
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

#define MASK 0xf0

struct termios tty;
int count=0;

int set_interface_attribs(int fd, int speed)
{
	if (tcgetattr(fd, &tty) < 0) {
		printf("Error from tcgetattr: %s\n", strerror(errno));
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
		printf("Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}


int decode_packet(char *p)
{
	char flag=p[0];
	flag &= ~(MASK);
	switch (flag)
	{
		case bla: break;
		case blu: break;

	}


}

int main(void)
{
	char *portname = "/dev/cuaU0";
	int fd, rdlen;
	unsigned char buf[80];
	unsigned char p_buf[80];

	fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		printf("Error opening %s: %s\n", portname, strerror(errno));
		return -1;
	}

	/*baudrate 2400, 8 bits, no parity, 1 stop bit */
	set_interface_attribs(fd, B2400);

	/* Waiting for 0xf1 : the last byte from last packet */
	fprintf (stderr,"Waiting the end of last packet... ");
	while(1)
	{
		rdlen = read(fd, buf, sizeof(buf) - 1);
		if ( rdlen == 1 && buf[0] == 0xf1 ) {
			fprintf(stdout, "ok\n");
			break;
		}
	}

	/* Now, we get 15 characters at a time */
	tty.c_cc[VMIN] = 15;
	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		printf("Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	memset ( buf, '\0', sizeof buf);
	memset ( p_buf, '\0', sizeof p_buf);

	/* Main loop */
	while(1)
	{
		rdlen = read(fd, buf, sizeof(buf) - 1);
		if (rdlen > 0) {
			if (memcmp(p_buf, buf, rdlen) != 0)
			{
				count++;
				memcpy(p_buf, buf, rdlen);
				unsigned char   *p;
				printf("%05d : Read %d:", count, rdlen);
				for (p = buf; rdlen-- > 0; p++)
					printf(" 0x%x", *p);
				printf("\n");
			}
		} else if (rdlen < 0) {
			printf("Error from read: %d: %s\n", rdlen, strerror(errno));
		}
	}
	return 0;
}

