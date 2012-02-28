/*
Copyright (C) 2008-2012 Vana Development Team

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

Decoder::Decoder(bool encrypted) :
	m_encrypted(encrypted)
{
}

void Decoder::encrypt(unsigned char *buffer, int32_t size) {
	if (!isEncrypted()) {
		return;
	}

	// Custom encryption layer
	int32_t j;
	uint8_t a, c;
	for (uint8_t i = 0; i < 3; ++i) {
		a = 0;
		for (j = size; j > 0; --j) {
			c = buffer[size - j];
			c = BitUtilities::RotateLeft(c, 3);
			c = (uint8_t)(c + j); // Guess this is supposed to be right?
			c = c ^ a;
			a = c;
			c = BitUtilities::RotateRight(a, j);
			c = c ^ 0xFF;
			c = c + 0x48;
			buffer[size - j] = c;
		}
		a = 0;
		for (j = size; j > 0; --j) {
			c = buffer[j - 1];
			c = BitUtilities::RotateLeft(c, 4);
			c = (uint8_t)(c + j); // Guess this is supposed to be right?
			c = c ^ a;
			a = c;
			c = c ^ 0x13;
			c = BitUtilities::RotateRight(c, 3);
			buffer[j - 1] = c;
		}
	}

	// Standard AES
	int32_t pos = 0;
	uint8_t first = 1;
	int32_t tPos = 0;
	CryptoPP::OFB_Mode<CryptoPP::AES>::Encryption ofbEncryption;

	while (size > pos) {
		ofbEncryption.SetKeyWithIV(AesKey, AesKeySize, m_send.getBytes()); // Need to set it before every encryption

		tPos = 1460 - first * 4;
		if (size > (pos + tPos)) {
			ofbEncryption.ProcessData(buffer + pos, buffer + pos, tPos);
		}
		else {
			ofbEncryption.ProcessData(buffer + pos, buffer + pos, size - pos);
		}
		pos += tPos;
		if (first) {
			first = 0;
		}
	}

	m_send.shuffle();
}

void Decoder::decrypt(unsigned char *buffer, int32_t size) {
	if (!isEncrypted()) {
		return;
	}

	// Standard AES
	int32_t pos = 0;
	uint8_t first = 1;
	int32_t tPos = 0;
	CryptoPP::OFB_Mode<CryptoPP::AES>::Decryption ofbDecryption;

	while (size > pos) {
		ofbDecryption.SetKeyWithIV(AesKey, AesKeySize, m_recv.getBytes()); // Need to set it before every decryption

		tPos = 1460 - first * 4;
		if (size > (pos + tPos)) {
			ofbDecryption.ProcessData(buffer + pos, buffer + pos, tPos);
		}
		else {
			ofbDecryption.ProcessData(buffer + pos, buffer + pos, size - pos);
		}
		pos += tPos;
		if (first) {
			first = 0;
		}
	}

	m_recv.shuffle();

	// Custom decryption layer
	int32_t j;
	uint8_t a, b, c;
	for (uint8_t i = 0; i < 3; i++) {
		a = 0;
		b = 0;
		for (j = size; j > 0; j--) {
			c = buffer[j - 1];
			c = BitUtilities::RotateLeft(c, 3);
			c = c ^ 0x13;
			a = c;
			c = c ^ b;
			c = (uint8_t)(c - j); // Guess this is supposed to be right?
			c = BitUtilities::RotateRight(c, 4);
			b = a;
			buffer[j - 1] = c;
		}
		a = 0;
		b = 0;
		for (j = size; j > 0; j--) {
			c = buffer[size - j];
			c = c - 0x48;
			c = c ^ 0xFF;
			c = BitUtilities::RotateLeft(c, j);
			a = c;
			c = c ^ b;
			c = (uint8_t)(c - j); // Guess this is supposed to be right?
			c = BitUtilities::RotateRight(c, 3);
			b = a;
			buffer[size - j] = c;
		}
	}
}

void Decoder::createHeader(unsigned char *header, uint16_t size) {
	uint16_t version = 0;
	uint16_t pSize = 0;
	if (isEncrypted()) {
		unsigned char *iv = m_send.getBytes();
		version = ((iv[3] << 8) | iv[2]);
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
	m_recv.updateIv(Randomizer::Instance()->randInt());
	m_send.updateIv(Randomizer::Instance()->randInt());

	PacketCreator packet;
	packet.add<header_t>(patchLocation != "" ? IV_PATCH_LOCATION : IV_NO_PATCH_LOCATION);
	packet.add<uint16_t>(MapleVersion::Version);
	packet.addString(patchLocation);
	packet.add<uint32_t>(m_recv.getIv());
	packet.add<uint32_t>(m_send.getIv());
	packet.add<int8_t>(MapleVersion::Locale);

	return packet;
}