#ifndef BUFFERUTILITIES_H
#define BUFFERUTILITIES_H

#include <string.h>

short getShort(unsigned char* buf);
int getInt(unsigned char* buf);
void getString(unsigned char* buf, int len, char* out);

#endif