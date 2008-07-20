/*
Copyright (C) 2008 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "Decoder.h"
#include "AESEncryption.h"
#include "MapleEncryption.h"
#include "Randomizer.h"
#include "SendHeader.h"
#include "PacketCreator.h"

void Decoder::encrypt(unsigned char *buffer, int size) {
	MapleEncryption::mapleEncrypt(buffer, size);
	int pos=0,first=1;
	while (size > pos) {
		if (size>pos+1460-first*4) {
			decryptofb(buffer+pos, Decoder::ivSend, 1460 - first*4);
		}
		else
			decryptofb(buffer+pos, Decoder::ivSend, size-pos);
		pos+=1460-first*4;
		if (first)
			first=0;
	}
} 
 
void Decoder::next() {
	MapleEncryption::nextIV(Decoder::ivSend);
}

void Decoder::decrypt(unsigned char *buffer, int size) {
	decryptofb(buffer, Decoder::ivRecv, size);
	MapleEncryption::nextIV(Decoder::ivRecv); 
	MapleEncryption::mapleDecrypt(buffer, size);
}

void Decoder::createHeader (unsigned char *header, short size) {
	short a = ivSend[3]*0x100 + ivSend[2];
	a = a ^ -(MAPLE_VERSION+1);
	short b = a ^ size; 
	header[0] = a%0x100;
	header[1] = (a-header[0])/0x100;
	header[2] = b%0x100;
	header[3] = (b-header[2])/0x100;
}

Packet & Decoder::getConnectPacket(string unknown) {
	(*(int *)ivRecv) = Randomizer::Instance()->randInt();
	(*(int *)ivSend) = Randomizer::Instance()->randInt();

	Packet packet;
	packet.addShort(0); // Packet len, this will be added later in the packet
	packet.addShort(MAPLE_VERSION);
	packet.addString(unknown); // Unknown, the official login server sends a "0", the channel server sends nothing
	packet.addInt(*(int *) ivRecv);
	packet.addInt(*(int *) ivSend);
	packet.addByte(0x08);

	packet.setShort(packet.getSize() - 2, 0); // -2 as the size does not include the size of the size header


	Packet &ret = packet;
	return ret;
}
