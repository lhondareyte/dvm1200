/*
 * Copyright (c) 2017 Luc Hondareyte
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

char d_decode(uint8_t d) 
{
	char rc;
	switch(d) {
		/* Numbers */ 
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
}

void u_decode(uint8_t s, uint8_t u, uint8_t x) 
{
	/* Temperature */ 
	if ( u == 0x00 ) {
		if ( x == 0x21 ) fprintf(stdout, "C°");
		if ( x == 0x11 ) fprintf(stdout, "F°");
	} else {
		/* Scaling */ 
		switch (s) {
			case 0x01: fprintf(stdout,"m"); break;
			case 0x04: fprintf(stdout,"M"); break;
			case 0x1a: fprintf(stdout,"u"); break;
			case 0x10: fprintf(stdout,"u"); break;
			case 0x20: fprintf(stdout,"n"); break;
			case 0x40: fprintf(stdout,"K"); break;
		}
		/* Units */
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
