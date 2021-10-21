/*
 * Copyright (c) 2017 Luc Hondareyte
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

char d_decode(uint8_t d) {
	char rc;
	switch(d)
	{

		// Numbers
		case 0x0a: rc='1'; break;
		case 0xad: rc='2'; break;
		case 0x8f: rc='3'; break;
		case 0x4e: rc='4'; break;
		case 0xc7: rc='5'; break;
		case 0xe7: rc='6'; break;
		case 0x8a: rc='7'; break;
		case 0xef: rc='8'; break;
		case 0xcf: rc='9'; break;
		case 0xeb: rc='0'; break;
		case 0x61: rc='L'; break;
		default:   rc=1; break;
	}
	return rc;
};

void u_decode(uint8_t s, uint8_t u, uint8_t x) {
	// Temperature
	if ( u == 0x00 ) {
		if ( x == 0x21 ) fprintf(stdout, "C°");
		if ( x == 0x11 ) fprintf(stdout, "F°");
	} else {
		// Scaling
		switch (s) {
			case 0x01: fprintf(stdout,"m"); break;
			case 0x04: fprintf(stdout,"M"); break;
			case 0x1a: fprintf(stdout,"u"); break;
			case 0x10: fprintf(stdout,"u"); break;
			case 0x20: fprintf(stdout,"n"); break;
			case 0x40: fprintf(stdout,"K"); break;
		}
		// Units
		switch (u) {
			case 0x01: fprintf(stdout,"A"); break;
			case 0x02: fprintf(stdout,"V"); break;
			case 0x04: fprintf(stdout,"Hz"); break;
			case 0x10: fprintf(stdout,"F"); break;
			case 0x20: fprintf(stdout,"Ohms"); break;
			case 0x50: fprintf(stdout,"F"); break;
		}
	}
}
