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

#include <stdio.h>
#include <stdlib.h>
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
