#ifndef PGMSPACE_UTILS_H_INCLUDED
#define PGMSPACE_UTILS_H_INCLUDED

#include <avr/pgmspace.h>

void strcpypgm(char* dst, const char* src);
void memcpypgm(char* dst, const char* src, unsigned int num);

#endif // PGMSPACE_UTILS_H_INCLUDED