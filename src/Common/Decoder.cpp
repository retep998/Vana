/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "MapleEncryption.h"
#include "MapleVersion.h"
#include "PacketCreator.h"
#include "Randomizer.h"
#include "SendHeader.h"
// CryptoPP
#include <aes.h>
#include <modes.h>

const uint8_t AesKeySize = 32;
const uint8_t AesKey[AesKeySize] = {
	0x13, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0xB4, 0x00, 0x00, 0x00,
	0x1B, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00
};

void Decoder::encrypt(unsigned char *buffer, int32_t size) {
	MapleEncryption::mapleEncrypt(buffer, size);
	int32_t pos = 0;
	uint8_t first = 1;

	CryptoPP::OFB_Mode<CryptoPP::AES>::Encryption ofbEncryption;

	while (size > pos) {
		ofbEncryption.SetKeyWithIV(AesKey, AesKeySize, ivSend); // Need to set it before every encryption

		if (size > (pos + 1460 - first * 4)) {
			ofbEncryption.ProcessData(buffer + pos, buffer + pos, 1460 - first * 4);
		}
		else {
			ofbEncryption.ProcessData(buffer + pos, buffer + pos, size - pos);
		}
		pos += 1460 - first * 4;
		if (first == 1)
			first = 0;
	}
}
 
void Decoder::next() {
	MapleEncryption::nextIv(ivSend);
}

void Decoder::decrypt(unsigned char *buffer, int32_t size) {
	int32_t pos = 0;
	uint8_t first = 1;

	CryptoPP::OFB_Mode<CryptoPP::AES>::Decryption ofbDecryption;

	while (size > pos) {
		ofbDecryption.SetKeyWithIV(AesKey, AesKeySize, ivRecv); // Need to set it before every decryption

		if (size > (pos + 1460 - first * 4)) {
			ofbDecryption.ProcessData(buffer + pos, buffer + pos, 1460 - first * 4);
		}
		else {
			ofbDecryption.ProcessData(buffer + pos, buffer + pos, size - pos);
		}
		pos += 1460 - first * 4;
		if (first == 1)
			first = 0;
	}
	
	MapleEncryption::nextIv(ivRecv);
	MapleEncryption::mapleDecrypt(buffer, size);
}

void Decoder::createHeader(unsigned char *header, int16_t size) {
	int16_t a = ivSend[3] * 0x100 + ivSend[2];
	a = a ^ -(MAPLE_VERSION + 1);
	int16_t b = a ^ size;
	header[0] = a % 0x100;
	header[1] = (a - header[0]) / 0x100;
	header[2] = b % 0x100;
	header[3] = (b - header[2]) / 0x100;
}

PacketCreator Decoder::getConnectPacket(string unknown) {
	(*(int32_t*)ivRecv) = Randomizer::Instance()->randInt();
	(*(int32_t*)ivSend) = Randomizer::Instance()->randInt();
	// Use the setter to prepare the IV
	setIvRecv(ivRecv);
	setIvSend(ivSend);

	PacketCreator packet;
	packet.add<int16_t>(0); // Packet len, this will be added later in the packet
	packet.add<int16_t>(MAPLE_VERSION);
	packet.addString(unknown); // Unknown, the official login server sends a "0", the channel server sends nothing
	packet.add<int32_t>(*(int32_t*) ivRecv);
	packet.add<int32_t>(*(int32_t*) ivSend);
	packet.add<int8_t>(MAPLE_LOCALE);

	packet.set<int16_t>(packet.getSize() - 2, 0); // -2 as the size does not include the size of the size header

	return packet;
}
