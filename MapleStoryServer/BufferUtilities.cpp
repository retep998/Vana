#include "BufferUtilities.h"

short getShort(unsigned char* buf){
	return buf[0] + buf[1]*0x100;
}

int getInt(unsigned char* buf){
	return buf[0] + buf[1]*0x100 + buf[2]*0x100*0x100 + buf[3]*0x100*0x100*0x100;
}

void getString(unsigned char* buf, int len, char* out){
	strncpy_s(out, len+1, (const char*)buf, len); 
}
