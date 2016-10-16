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
#include "encrypted_packet_transformer.hpp"
#include "common/common_header.hpp"
#include "common/maple_version.hpp"
#include "common/packet_builder.hpp"
#include "common/util/bit.hpp"
#include "common/util/randomizer.hpp"
#include <botan/filters.h>
#include <botan/lookup.h>
#include <botan/pipe.h>

namespace vana {

const uint8_t aes_key_size = 32;
const uint8_t aes_key[aes_key_size] = {
	0x13, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00,
	0x06, 0x00, 0x00, 0x00,
	0xB4, 0x00, 0x00, 0x00,
	0x1B, 0x00, 0x00, 0x00,
	0x0F, 0x00, 0x00, 0x00,
	0x33, 0x00, 0x00, 0x00,
	0x52, 0x00, 0x00, 0x00,
};
const int32_t block_size = 1460;

encrypted_packet_transformer::encrypted_packet_transformer(crypto_iv recv_iv, crypto_iv send_iv) :
	m_recv{recv_iv},
	m_send{send_iv}
{
	m_botan_key = Botan::SymmetricKey{aes_key, aes_key_size};
}

auto encrypted_packet_transformer::test_packet(unsigned char *header) -> validity_result {
	uint16_t version = 0;
	uint16_t p_size = 0;
	get_version_and_size(header, version, p_size);
	// All packets are at least a header
	return version == maple_version::version && p_size >= sizeof(packet_header) ?
		validity_result::valid :
		validity_result::invalid;
}

auto encrypted_packet_transformer::get_packet_length(unsigned char *header) -> uint16_t {
	uint16_t version = 0;
	uint16_t p_size = 0;
	get_version_and_size(header, version, p_size);
	return p_size;
}

auto encrypted_packet_transformer::set_packet_header(unsigned char *header, uint16_t real_packet_size) -> void {
	auto iv = m_send.get_bytes();
	uint16_t version = ((iv[3] << 8) | iv[2]);
	version ^= -(maple_version::version + 1);

	uint16_t p_size = version ^ real_packet_size;

	*reinterpret_cast<uint16_t *>(header) = version;
	*reinterpret_cast<uint16_t *>(header + sizeof(uint16_t)) = p_size;
}

auto encrypted_packet_transformer::encrypt_packet(unsigned char *packet_data, int32_t real_packet_size, uint16_t header_size) -> void {
	// Custom encryption layer
	int32_t j;
	uint8_t a, c;
	for (uint8_t i = 0; i < 3; ++i) {
		a = 0;
		for (j = real_packet_size; j > 0; --j) {
			c = packet_data[real_packet_size - j];
			c = vana::util::bit::rotate_left(c, 3);
			c = static_cast<uint8_t>(c + j); // Guess this is supposed to be right?
			c = c ^ a;
			a = c;
			c = vana::util::bit::rotate_right(a, j);
			c = c ^ 0xFF;
			c = c + 0x48;
			packet_data[real_packet_size - j] = c;
		}
		a = 0;
		for (j = real_packet_size; j > 0; --j) {
			c = packet_data[j - 1];
			c = vana::util::bit::rotate_left(c, 4);
			c = static_cast<uint8_t>(c + j); // Guess this is supposed to be right?
			c = c ^ a;
			a = c;
			c = c ^ 0x13;
			c = vana::util::bit::rotate_right(c, 3);
			packet_data[j - 1] = c;
		}
	}

	// Standard AES
	int32_t pos = 0;
	uint8_t first = 1;
	int32_t t_pos = 0;
	int32_t write_amount = 0;
	Botan::InitializationVector iv{m_send.get_bytes(), 16};

	while (real_packet_size > pos) {
		t_pos = block_size - first * header_size;
		write_amount = real_packet_size > (pos + t_pos) ?
			t_pos :
			(real_packet_size - pos);

		Botan::Pipe pipe{Botan::get_cipher("AES-256/OFB/NoPadding", m_botan_key, iv, Botan::ENCRYPTION)};
		pipe.start_msg();
		pipe.write(packet_data + pos, write_amount);
		pipe.end_msg();

		// Process the message and write it into the buffer
		pipe.read(packet_data + pos, write_amount);

		pos += t_pos;
		if (first) {
			first = 0;
		}
	}

	m_send.shuffle();
}

auto encrypted_packet_transformer::decrypt_packet(unsigned char *packet_data, int32_t real_packet_size, uint16_t header_size) -> void {
	// Standard AES
	int32_t pos = 0;
	uint8_t first = 1;
	int32_t t_pos = 0;
	int32_t read_amount = 0;
	Botan::InitializationVector iv{m_recv.get_bytes(), 16};

	while (real_packet_size > pos) {
		t_pos = block_size - first * header_size;
		read_amount = real_packet_size > (pos + t_pos) ?
			t_pos :
			(real_packet_size - pos);

		Botan::Pipe pipe{Botan::get_cipher("AES-256/OFB/NoPadding", m_botan_key, iv, Botan::DECRYPTION)};
		pipe.start_msg();
		pipe.write(packet_data + pos, read_amount);
		pipe.end_msg();

		// Process the message and write it into the buffer
		pipe.read(packet_data + pos, read_amount);

		pos += t_pos;
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
		for (j = real_packet_size; j > 0; j--) {
			c = packet_data[j - 1];
			c = vana::util::bit::rotate_left(c, 3);
			c = c ^ 0x13;
			a = c;
			c = c ^ b;
			c = static_cast<uint8_t>(c - j); // Guess this is supposed to be right?
			c = vana::util::bit::rotate_right(c, 4);
			b = a;
			packet_data[j - 1] = c;
		}
		a = 0;
		b = 0;
		for (j = real_packet_size; j > 0; j--) {
			c = packet_data[real_packet_size - j];
			c = c - 0x48;
			c = c ^ 0xFF;
			c = vana::util::bit::rotate_left(c, j);
			a = c;
			c = c ^ b;
			c = static_cast<uint8_t>(c - j); // Guess this is supposed to be right?
			c = vana::util::bit::rotate_right(c, 3);
			b = a;
			packet_data[real_packet_size - j] = c;
		}
	}
}

auto encrypted_packet_transformer::get_version_and_size(unsigned char *header, uint16_t &version, uint16_t &size) -> void {
	auto iv = m_recv.get_bytes();
	uint16_t enc = ((iv[3] << 8) | iv[2]);
	version = (-(*reinterpret_cast<uint16_t *>(header)) - 1) ^ enc;
	size = *reinterpret_cast<uint16_t *>(header) ^ *reinterpret_cast<uint16_t *>(header + sizeof(uint16_t));
}

}