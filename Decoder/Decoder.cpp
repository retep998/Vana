#include "Decoder.h"
#include "AESEncryption.h"
#include "MapleEncryption.h"

void Decoder::encrypt(unsigned char *buffer, int size){
	MapleEncryption::mapleEncrypt(buffer, size);
	int pos=0,first=1;
	while(size > pos){
		if(size>pos+1460-first*4){
			decryptofb(buffer+pos, Decoder::ivSend, 1460 - first*4);
		}
		else
			decryptofb(buffer+pos, Decoder::ivSend, size-pos);
		pos+=1460-first*4;
		if(first)
			first=0;
	}
} 
 
void Decoder::next(){
	MapleEncryption::nextIV(Decoder::ivSend);
}

void Decoder::decrypt(unsigned char *buffer, int size){
	decryptofb(buffer, Decoder::ivRecv, size);
	MapleEncryption::nextIV(Decoder::ivRecv); 
	MapleEncryption::mapleDecrypt(buffer, size);
}

void Decoder::createHeader (unsigned char* header, short size) {
	short a = ivSend[3]*0x100 + ivSend[2];
	a = a ^ -54;
	short b = a ^ size; 
	header[0] = a%0x100;
	header[1] = (a-header[0])/0x100;
	header[2] = b%0x100;
	header[3] = (b-header[2])/0x100;
}