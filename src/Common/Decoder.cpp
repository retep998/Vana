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
		if (first) {
			first = 0;
		}
	}
}

void Decoder::next() {
	MapleEncryption::nextIv(ivSend);
}

void Decoder::decrypt(unsigned char *buffer, int32_t size) {
	int32_t pos = 0;
	uint8_t first = 1;
	int32_t tpos = 0;

	CryptoPP::OFB_Mode<CryptoPP::AES>::Decryption ofbDecryption;

	while (size > pos) {
		ofbDecryption.SetKeyWithIV(AesKey, AesKeySize, ivRecv); // Need to set it before every decryption

		tpos = 1460 - first * 4;
		if (size > (pos + tpos)) {
			ofbDecryption.ProcessData(buffer + pos, buffer + pos, tpos);
		}
		else {
			ofbDecryption.ProcessData(buffer + pos, buffer + pos, size - pos);
		}
		pos += tpos;
		if (first) {
			first = 0;
		}
	}

	MapleEncryption::nextIv(ivRecv);
	MapleEncryption::mapleDecrypt(buffer, size);
}

void Decoder::createHeader(unsigned char *header, int16_t size) {
	int16_t a = ivSend[3] * 0x100 + ivSend[2];
	a = a ^ -(MapleVersion::Version + 1);
	int16_t b = a ^ size;
	header[0] = a % 0x100;
	header[1] = (a - header[0]) / 0x100;
	header[2] = b % 0x100;
	header[3] = (b - header[2]) / 0x100;
}

PacketCreator Decoder::getConnectPacket(const string &patchLocation) {
	(*(uint32_t*)ivRecv) = Randomizer::Instance()->randInt();
	(*(uint32_t*)ivSend) = Randomizer::Instance()->randInt();
	// Use the setter to prepare the IV
	setIvRecv(ivRecv);
	setIvSend(ivSend);

	PacketCreator packet;
	packet.add<int16_t>(patchLocation != "" ? IV_PATCH_LOCATION : IV_NO_PATCH_LOCATION);
	packet.add<int16_t>(MapleVersion::Version);
	packet.addString(patchLocation);
	packet.add<uint32_t>(*(uint32_t*) ivRecv);
	packet.add<uint32_t>(*(uint32_t*) ivSend);
	packet.add<int8_t>(MapleVersion::Locale);

	return packet;
}
