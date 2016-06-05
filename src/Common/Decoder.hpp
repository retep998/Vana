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

#include "Common/BlockCipherIv.hpp"
#include "Common/MapleVersion.hpp"
#include "Common/Types.hpp"
#include <botan/filters.h>
#include <botan/lookup.h>
#include <botan/pipe.h>
#include <string>

namespace vana {
	class packet_builder;

	class decoder {
	public:
		explicit decoder(bool encrypted);

		auto valid_packet(unsigned char *header) -> bool;
		auto get_length(unsigned char *header) -> uint16_t;

		auto create_header(unsigned char *header, uint16_t size) -> void;
		auto encrypt(unsigned char *buffer, int32_t size, uint16_t header_len) -> void;
		auto decrypt(unsigned char *buffer, int32_t size, uint16_t header_len) -> void;
		auto set_recv_iv(crypto_iv iv) -> void { m_recv.update_iv(iv); }
		auto set_send_iv(crypto_iv iv) -> void { m_send.update_iv(iv); }
		auto get_recv_iv() const -> crypto_iv { return m_recv.get_iv(); }
		auto get_send_iv() const -> crypto_iv { return m_send.get_iv(); }
	private:
		auto get_version_and_size(unsigned char *header, uint16_t &version, uint16_t &size) -> void;

		block_cipher_iv m_recv;
		block_cipher_iv m_send;
		bool m_encrypted = true;
		Botan::OctetString m_botan_key;
	};

	inline
	auto decoder::get_length(unsigned char *header) -> uint16_t {
		uint16_t version = 0;
		uint16_t p_size = 0;
		get_version_and_size(header, version, p_size);
		return p_size;
	}

	inline
	auto decoder::valid_packet(unsigned char *header) -> bool {
		uint16_t version = 0;
		uint16_t p_size = 0;
		get_version_and_size(header, version, p_size);
		return version == maple_version::version && p_size >= 2;
	}

	inline
	auto decoder::get_version_and_size(unsigned char *header, uint16_t &version, uint16_t &size) -> void {
		if (!m_encrypted) {
			version = *reinterpret_cast<uint16_t *>(header);
			size = *reinterpret_cast<uint16_t *>(header + 2);
		}
		else {
			auto iv = m_recv.get_bytes();
			uint16_t enc = ((iv[3] << 8) | iv[2]);
			version = (-(*reinterpret_cast<uint16_t *>(header)) - 1) ^ enc;
			size = *reinterpret_cast<uint16_t *>(header) ^ *reinterpret_cast<uint16_t *>(header + 2);
		}
	}
}