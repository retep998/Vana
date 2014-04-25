/*
Copyright (C) 2008-2014 Vana Development Team

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
#include "Decoder.hpp"
#include "BitUtilities.hpp"
#include "CommonHeader.hpp"
#include "MapleVersion.hpp"
#include "PacketBuilder.hpp"
#include "Randomizer.hpp"

const uint8_t AesKeySize = 32;
const uint8_t AesKey[AesKeySize] = {
	0x13, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0xB4, 0x00, 0x00, 0x00,
	0x1B, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0x00, 0x52, 0x00, 0x00, 0x00
};
const int32_t BlockSize = 1460;

Decoder::Decoder(bool encrypted) :
	m_encrypted(encrypted)
{
	m_botanKey = Botan::SymmetricKey(AesKey, AesKeySize);
}

auto Decoder::encrypt(unsigned char *buffer, int32_t size, uint16_t headerLen) -> void {
	if (!m_encrypted) {
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
			c = static_cast<uint8_t>(c + j); // Guess this is supposed to be right?
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
			c = static_cast<uint8_t>(c + j); // Guess this is supposed to be right?
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
	int32_t writeAmount = 0;
	Botan::InitializationVector iv(m_send.getBytes(), 16);

	while (size > pos) {
		tPos = BlockSize - first * headerLen;
		writeAmount = (size > (pos + tPos) ? tPos : (size - pos));

		Botan::Pipe pipe(Botan::get_cipher("AES-256/OFB/NoPadding", m_botanKey, iv, Botan::ENCRYPTION));
		pipe.start_msg();
		pipe.write(buffer + pos, writeAmount);
		pipe.end_msg();

		// Process the message and write it into the buffer
		pipe.read(buffer + pos, writeAmount);

		pos += tPos;
		if (first) {
			first = 0;
		}
	}

	m_send.shuffle();
}

auto Decoder::decrypt(unsigned char *buffer, int32_t size, uint16_t headerLen) -> void {
	if (!m_encrypted) {
		return;
	}

	// Standard AES
	int32_t pos = 0;
	uint8_t first = 1;
	int32_t tPos = 0;
	int32_t readAmount = 0;
	Botan::InitializationVector iv(m_recv.getBytes(), 16);

	while (size > pos) {
		tPos = BlockSize - first * headerLen;
		readAmount = (size > (pos + tPos) ? tPos : (size - pos));

		Botan::Pipe pipe(Botan::get_cipher("AES-256/OFB/NoPadding", m_botanKey, iv, Botan::DECRYPTION));
		pipe.start_msg();
		pipe.write(buffer + pos, readAmount);
		pipe.end_msg();

		// Process the message and write it into the buffer
		pipe.read(buffer + pos, readAmount);

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
			c = static_cast<uint8_t>(c - j); // Guess this is supposed to be right?
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
			c = static_cast<uint8_t>(c - j); // Guess this is supposed to be right?
			c = BitUtilities::RotateRight(c, 3);
			b = a;
			buffer[size - j] = c;
		}
	}
}

auto Decoder::createHeader(unsigned char *header, uint16_t size) -> void {
	uint16_t version = 0;
	uint16_t pSize = 0;
	if (m_encrypted) {
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