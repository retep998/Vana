#ifndef MAPLEENC_H
#define MAPLEENC_H

class MapleEncryption {
public:
	static void nextIV(unsigned char* vector);
	static void mapleDecrypt(unsigned char* buf, int size);
	static void mapleEncrypt(unsigned char* buf, int size);
	static unsigned char ror(unsigned char val, int num);
	static unsigned char rol(unsigned char val, int num);
};

#endif