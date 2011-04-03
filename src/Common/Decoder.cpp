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
#include "BitUtilities.h"
#include "MapleVersion.h"
#include "PacketCreator.h"
#include "Randomizer.h"
#include "SmsgHeader.h"
#include <aes.h>
#include <modes.h>

const uint8_t AesKeySize = 32;
const uint8_t AesKey[AesKeySize] = {
	0x13, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0xB4, 0x00, 0x00, 0x00,
	0x1B, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00
};
const uint8_t ivTable[256] = {
	0xEC, 0x3F, 0x77, 0xA4, 0x45, 0xD0, 0x71, 0xBF, 0xB7, 0x98, 0x20, 0xFC, 0x4B, 0xE9, 0xB3, 0xE1,
	0x5C, 0x22, 0xF7, 0x0C,	0x44, 0x1B, 0x81, 0xBD, 0x63, 0x8D, 0xD4, 0xC3, 0xF2, 0x10, 0x19, 0xE0,
	0xFB, 0xA1, 0x6E, 0x66, 0xEA, 0xAE, 0xD6, 0xCE, 0x06, 0x18, 0x4E, 0xEB, 0x78, 0x95, 0xDB, 0xBA,
	0xB6, 0x42, 0x7A, 0x2A, 0x83, 0x0B, 0x54, 0x67, 0x6D, 0xE8, 0x65, 0xE7, 0x2F, 0x07, 0xF3, 0xAA,
	0x27, 0x7B, 0x85, 0xB0,	0x26, 0xFD, 0x8B, 0xA9, 0xFA, 0xBE, 0xA8, 0xD7, 0xCB, 0xCC, 0x92, 0xDA,
	0xF9, 0x93, 0x60, 0x2D,	0xDD, 0xD2, 0xA2, 0x9B, 0x39, 0x5F, 0x82, 0x21, 0x4C, 0x69, 0xF8, 0x31,
	0x87, 0xEE, 0x8E, 0xAD, 0x8C, 0x6A, 0xBC, 0xB5, 0x6B, 0x59, 0x13, 0xF1, 0x04, 0x00, 0xF6, 0x5A,
	0x35, 0x79, 0x48, 0x8F,	0x15, 0xCD, 0x97, 0x57, 0x12, 0x3E, 0x37, 0xFF, 0x9D, 0x4F, 0x51, 0xF5,
	0xA3, 0x70, 0xBB, 0x14,	0x75, 0xC2, 0xB8, 0x72, 0xC0, 0xED, 0x7D, 0x68, 0xC9, 0x2E, 0x0D, 0x62,
	0x46, 0x17, 0x11, 0x4D,	0x6C, 0xC4, 0x7E, 0x53, 0xC1, 0x25, 0xC7, 0x9A, 0x1C, 0x88, 0x58, 0x2C,
	0x89, 0xDC, 0x02, 0x64,	0x40, 0x01, 0x5D, 0x38, 0xA5, 0xE2, 0xAF, 0x55, 0xD5, 0xEF, 0x1A, 0x7C,
	0xA7, 0x5B, 0xA6, 0x6F,	0x86, 0x9F, 0x73, 0xE6, 0x0A, 0xDE, 0x2B, 0x99, 0x4A, 0x47, 0x9C, 0xDF,
	0x09, 0x76, 0x9E, 0x30,	0x0E, 0xE4, 0xB2, 0x94, 0xA0, 0x3B, 0x34, 0x1D, 0x28, 0x0F, 0x36, 0xE3,
	0x23, 0xB4, 0x03, 0xD8, 0x90, 0xC8, 0x3C, 0xFE, 0x5E, 0x32, 0x24, 0x50, 0x1F, 0x3A, 0x43, 0x8A,
	0x96, 0x41, 0x74, 0xAC,	0x52, 0x33, 0xF0, 0xD9, 0x29, 0x80, 0xB1, 0x16, 0xD3, 0xAB, 0x91, 0xB9,
	0x84, 0x7F, 0x61, 0x1E,	0xCF, 0xC5, 0xD1, 0x56, 0x3D, 0xCA, 0xF4, 0x05,	0xC6, 0xE5, 0x08, 0x49
};

Decoder::Decoder(bool encrypted) :
	m_encrypted(encrypted)
{
}

void Decoder::encrypt(unsigned char *buffer, int32_t size) {
	if (!isEncrypted()) {
		return;
	}
	mapleEncrypt(buffer, size);

	int32_t pos = 0;
	uint8_t first = 1;
	int32_t tpos = 0;
	CryptoPP::OFB_Mode<CryptoPP::AES>::Encryption ofbEncryption;

	while (size > pos) {
		ofbEncryption.SetKeyWithIV(AesKey, AesKeySize, m_ivSend); // Need to set it before every encryption

		tpos = 1460 - first * 4;
		if (size > (pos + tpos)) {
			ofbEncryption.ProcessData(buffer + pos, buffer + pos, tpos);
		}
		else {
			ofbEncryption.ProcessData(buffer + pos, buffer + pos, size - pos);
		}
		pos += tpos;
		if (first) {
			first = 0;
		}
	}

	nextIv(m_ivSend);
}

void Decoder::decrypt(unsigned char *buffer, int32_t size) {
	if (!isEncrypted()) {
		return;
	}
	int32_t pos = 0;
	uint8_t first = 1;
	int32_t tpos = 0;
	CryptoPP::OFB_Mode<CryptoPP::AES>::Decryption ofbDecryption;

	while (size > pos) {
		ofbDecryption.SetKeyWithIV(AesKey, AesKeySize, m_ivRecv); // Need to set it before every decryption

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

	nextIv(m_ivRecv);
	mapleDecrypt(buffer, size);
}

void Decoder::nextIv(unsigned char *vector) {
	uint8_t x[4] = {0xF2, 0x53, 0x50, 0xC6};
	uint8_t input;
	uint8_t valueInput;
	uint32_t fullIv;
	uint32_t shift;

	for (uint8_t i = 0; i < 4; i++) {
		input = vector[i];
		valueInput = ivTable[input];

		x[0] += (ivTable[x[1]] - input);
		x[1] -= (x[2] ^ valueInput);
		x[2] ^= (ivTable[x[3]] + input);
		x[3] -= (x[0] - valueInput);

		fullIv = (x[3] << 24) | (x[2] << 16) | (x[1] << 8) | x[0];
		shift = (fullIv >> 0x1D) | (fullIv << 0x03);

		x[0] = static_cast<uint8_t>(shift & 0xFFu);
		x[1] = static_cast<uint8_t>((shift >> 8) & 0xFFu);
		x[2] = static_cast<uint8_t>((shift >> 16) & 0xFFu);
		x[3] = static_cast<uint8_t>((shift >> 24) & 0xFFu);
	}

	setIv(vector, x);
}

void Decoder::mapleEncrypt(unsigned char *buf, int32_t size) {
	int32_t j;
	uint8_t a, c;
	for (uint8_t i = 0; i < 3; i++) {
		a = 0;
		for (j = size; j > 0; j--) {
			c = buf[size - j];
			c = BitUtilities::RotateLeft(c, 3);
			c = (uint8_t)(c + j); // Guess this is supposed to be right?
			c = c ^ a;
			a = c;
			c = BitUtilities::RotateRight(a, j);
			c = c ^ 0xFF;
			c = c + 0x48;
			buf[size - j] = c;
		}
		a = 0;
		for (j = size; j > 0; j--) {
			c = buf[j - 1];
			c = BitUtilities::RotateLeft(c, 4);
			c = (uint8_t)(c + j); // Guess this is supposed to be right?
			c = c ^ a;
			a = c;
			c = c ^ 0x13;
			c = BitUtilities::RotateRight(c, 3);
			buf[j - 1] = c;
		}
	}
}

void Decoder::mapleDecrypt(unsigned char *buf, int32_t size) {
	int32_t j;
	uint8_t a, b, c;
	for (uint8_t i = 0; i < 3; i++) {
		a = 0;
		b = 0;
		for (j = size; j > 0; j--) {
			c = buf[j - 1];
			c = BitUtilities::RotateLeft(c, 3);
			c = c ^ 0x13;
			a = c;
			c = c ^ b;
			c = (uint8_t)(c - j); // Guess this is supposed to be right?
			c = BitUtilities::RotateRight(c, 4);
			b = a;
			buf[j - 1] = c;
		}
		a = 0;
		b = 0;
		for (j = size; j > 0; j--) {
			c = buf[size - j];
			c = c - 0x48;
			c = c ^ 0xFF;
			c = BitUtilities::RotateLeft(c, j);
			a = c;
			c = c ^ b;
			c = (uint8_t)(c - j); // Guess this is supposed to be right?
			c = BitUtilities::RotateRight(c, 3);
			b = a;
			buf[size - j] = c;
		}
	}
}

void Decoder::createHeader(unsigned char *header, uint16_t size) {
	uint16_t version = 0;
	uint16_t pSize = 0;
	if (isEncrypted()) {
		version = ((m_ivSend[3] << 8) | m_ivSend[2]);
		version ^= -(MapleVersion::Version + 1);
		pSize = version ^ size;
	}
	else {
		version = MapleVersion::Version;
		pSize = size;
	}
	(*(uint16_t *)(header)) = version;
	(*(uint16_t *)(header + 2)) = pSize;
}

PacketCreator Decoder::getConnectPacket(const string &patchLocation) {
	(*(uint32_t*)(m_ivRecv)) = Randomizer::Instance()->randInt();
	(*(uint32_t*)(m_ivSend)) = Randomizer::Instance()->randInt();
	// Use the setter to prepare the IV
	setIvRecv(m_ivRecv);
	setIvSend(m_ivSend);

	PacketCreator packet;
	packet.addHeader(patchLocation != "" ? IV_PATCH_LOCATION : IV_NO_PATCH_LOCATION);
	packet.add<uint16_t>(MapleVersion::Version);
	packet.addString(patchLocation);
	packet.add<uint32_t>(*(uint32_t*) m_ivRecv);
	packet.add<uint32_t>(*(uint32_t*) m_ivSend);
	packet.add<int8_t>(MapleVersion::Locale);

	return packet;
}