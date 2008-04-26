#ifndef DECODER_H
#define DECODER_H

#define _CRT_RAND_S
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

class Decoder {
private:
	unsigned char ivRecv[4];
	unsigned char ivSend[4];
	unsigned char connectBuffer[15];

public:
	static const int CONNECT_LENGTH = 15;
	static int getLength (unsigned char* header) {
		return ((header[0] + header[1]*0x100) ^ (header[2] + header[3]*0x100)); 
	}
	void createHeader (unsigned char* header, short size); 

	unsigned char* getConnectPacket() {
		(*(short*)ivRecv) = rand();
		(*(short*)ivSend) = rand();
		(*(short*)(ivRecv+2)) = rand();
		(*(short*)(ivSend+2)) = rand();
		(*(short*)connectBuffer) = 0x000D;
		(*(int*)(connectBuffer+sizeof(short))) = 53;
		memcpy_s(connectBuffer+6, 4, ivRecv, 4);
		memcpy_s(connectBuffer+10, 4, ivSend, 4);
		connectBuffer[14] = 0x08;
		return connectBuffer;
	}
	void encrypt (unsigned char *buffer, int size);
	void decrypt (unsigned char *buffer, int size);
	void next ();
	static void Init();

};

#endif