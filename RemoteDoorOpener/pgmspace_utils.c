#include "pgmspace_utils.h"

void strcpypgm(char* dst, const char* src) {
	char chr = 0;
	do {
		chr = pgm_read_byte(src);
		*dst = chr;
		src++;
		dst++;
	} while(chr != '\0');
}

void memcpypgm(char* dst, const char* src, unsigned int num) {
	char chr = 0;
	do {
		chr = pgm_read_byte(src);
		*dst = chr;
		src++;
		dst++;
	} while(chr != '\0');
}