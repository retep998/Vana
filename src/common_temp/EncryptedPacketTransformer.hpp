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
#pragma once


#include "common_temp/BlockCipherIv.hpp"
#include "common_temp/PacketTransformer.hpp"
#include "common_temp/Types.hpp"
#include <botan/lookup.h>

namespace vana {
	class encrypted_packet_transformer final : public packet_transformer {
	public:
		encrypted_packet_transformer(crypto_iv recv_iv, crypto_iv send_iv);
		auto test_packet(unsigned char *header) -> validity_result override;
		auto get_packet_length(unsigned char *header) -> uint16_t override;
		auto set_packet_header(unsigned char *header, uint16_t real_packet_size) -> void override;
		auto encrypt_packet(unsigned char *packet_data, int32_t real_packet_size, uint16_t header_size) -> void override;
		auto decrypt_packet(unsigned char *packet_data, int32_t real_packet_size, uint16_t header_size) -> void override;
	private:
		auto get_version_and_size(unsigned char *header, uint16_t &version, uint16_t &size) -> void;

		block_cipher_iv m_recv;
		block_cipher_iv m_send;
		Botan::OctetString m_botan_key;
	};
}