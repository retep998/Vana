/*
Copyright (C) 2008-2016 Vana Development Team

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
#include "EncryptedPacketTransformer.hpp"
#include "Common/BitUtilities.hpp"
#include "Common/CommonHeader.hpp"
#include "Common/MapleVersion.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/Randomizer.hpp"
#include <botan/filters.h>
#include <botan/lookup.h>
#include <botan/pipe.h>

namespace Vana {

const uint8_t AesKeySize = 32;
const uint8_t AesKey[AesKeySize] = {
	0x13, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00,
	0x06, 0x00, 0x00, 0x00,
	0xB4, 0x00, 0x00, 0x00,
	0x1B, 0x00, 0x00, 0x00,
	0x0F, 0x00, 0x00, 0x00,
	0x33, 0x00, 0x00, 0x00,
	0x52, 0x00, 0x00, 0x00,
};
const int32_t BlockSize = 1460;

EncryptedPacketTransformer::EncryptedPacketTransformer(iv_t recvIv, iv_t sendIv) :
	m_recv{recvIv},
	m_send{sendIv}
{
	m_botanKey = Botan::SymmetricKey{AesKey, AesKeySize};
}

auto EncryptedPacketTransformer::testPacket(unsigned char *header) -> ValidityResult {
	uint16_t version = 0;
	uint16_t pSize = 0;
	getVersionAndSize(header, version, pSize);
	// All packets are at least a header
	return version == MapleVersion::Version && pSize >= sizeof(header_t) ?
		ValidityResult::Valid :
		ValidityResult::Invalid;
}

auto EncryptedPacketTransformer::getPacketLength(unsigned char *header) -> uint16_t {
	uint16_t version = 0;
	uint16_t pSize = 0;
	getVersionAndSize(header, version, pSize);
	return pSize;
}

auto EncryptedPacketTransformer::setPacketHeader(unsigned char *header, uint16_t realPacketSize) -> void {
	auto iv = m_send.getBytes();
	uint16_t version = ((iv[3] << 8) | iv[2]);
	version ^= -(MapleVersion::Version + 1);

	uint16_t pSize = version ^ realPacketSize;

	*reinterpret_cast<uint16_t *>(header) = version;
	*reinterpret_cast<uint16_t *>(header + sizeof(uint16_t)) = pSize;
}

auto EncryptedPacketTransformer::encryptPacket(unsigned char *packetData, int32_t realPacketSize, uint16_t headerSize) -> void {
	// Custom encryption layer
	int32_t j;
	uint8_t a, c;
	for (uint8_t i = 0; i < 3; ++i) {
		a = 0;
		for (j = realPacketSize; j > 0; --j) {
			c = packetData[realPacketSize - j];
			c = BitUtilities::rotateLeft(c, 3);
			c = static_cast<uint8_t>(c + j); // Guess this is supposed to be right?
			c = c ^ a;
			a = c;
			c = BitUtilities::rotateRight(a, j);
			c = c ^ 0xFF;
			c = c + 0x48;
			packetData[realPacketSize - j] = c;
		}
		a = 0;
		for (j = realPacketSize; j > 0; --j) {
			c = packetData[j - 1];
			c = BitUtilities::rotateLeft(c, 4);
			c = static_cast<uint8_t>(c + j); // Guess this is supposed to be right?
			c = c ^ a;
			a = c;
			c = c ^ 0x13;
			c = BitUtilities::rotateRight(c, 3);
			packetData[j - 1] = c;
		}
	}

	// Standard AES
	int32_t pos = 0;
	uint8_t first = 1;
	int32_t tPos = 0;
	int32_t writeAmount = 0;
	Botan::InitializationVector iv{m_send.getBytes(), 16};

	while (realPacketSize > pos) {
		tPos = BlockSize - first * headerSize;
		writeAmount = realPacketSize > (pos + tPos) ?
			tPos :
			(realPacketSize - pos);

		Botan::Pipe pipe{Botan::get_cipher("AES-256/OFB/NoPadding", m_botanKey, iv, Botan::ENCRYPTION)};
		pipe.start_msg();
		pipe.write(packetData + pos, writeAmount);
		pipe.end_msg();

		// Process the message and write it into the buffer
		pipe.read(packetData + pos, writeAmount);

		pos += tPos;
		if (first) {
			first = 0;
		}
	}

	m_send.shuffle();
}

auto EncryptedPacketTransformer::decryptPacket(unsigned char *packetData, int32_t realPacketSize, uint16_t headerSize) -> void {
	// Standard AES
	int32_t pos = 0;
	uint8_t first = 1;
	int32_t tPos = 0;
	int32_t readAmount = 0;
	Botan::InitializationVector iv{m_recv.getBytes(), 16};

	while (realPacketSize > pos) {
		tPos = BlockSize - first * headerSize;
		readAmount = realPacketSize > (pos + tPos) ?
			tPos :
			(realPacketSize - pos);

		Botan::Pipe pipe{Botan::get_cipher("AES-256/OFB/NoPadding", m_botanKey, iv, Botan::DECRYPTION)};
		pipe.start_msg();
		pipe.write(packetData + pos, readAmount);
		pipe.end_msg();

		// Process the message and write it into the buffer
		pipe.read(packetData + pos, readAmount);

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
		for (j = realPacketSize; j > 0; j--) {
			c = packetData[j - 1];
			c = BitUtilities::rotateLeft(c, 3);
			c = c ^ 0x13;
			a = c;
			c = c ^ b;
			c = static_cast<uint8_t>(c - j); // Guess this is supposed to be right?
			c = BitUtilities::rotateRight(c, 4);
			b = a;
			packetData[j - 1] = c;
		}
		a = 0;
		b = 0;
		for (j = realPacketSize; j > 0; j--) {
			c = packetData[realPacketSize - j];
			c = c - 0x48;
			c = c ^ 0xFF;
			c = BitUtilities::rotateLeft(c, j);
			a = c;
			c = c ^ b;
			c = static_cast<uint8_t>(c - j); // Guess this is supposed to be right?
			c = BitUtilities::rotateRight(c, 3);
			b = a;
			packetData[realPacketSize - j] = c;
		}
	}
}

auto EncryptedPacketTransformer::getVersionAndSize(unsigned char *header, uint16_t &version, uint16_t &size) -> void {
	auto iv = m_recv.getBytes();
	uint16_t enc = ((iv[3] << 8) | iv[2]);
	version = (-(*reinterpret_cast<uint16_t *>(header)) - 1) ^ enc;
	size = *reinterpret_cast<uint16_t *>(header) ^ *reinterpret_cast<uint16_t *>(header + sizeof(uint16_t));
}

}