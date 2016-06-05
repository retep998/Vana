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

#include "common/types_temp.hpp"

namespace vana {
	class packet_transformer {
	public:
		virtual auto test_packet(unsigned char *header) -> validity_result;
		virtual auto get_packet_length(unsigned char *header) -> uint16_t;
		virtual auto set_packet_header(unsigned char *header, uint16_t real_packet_size) -> void;
		virtual auto encrypt_packet(unsigned char *packet_data, int32_t real_packet_size, uint16_t header_size) -> void;
		virtual auto decrypt_packet(unsigned char *packet_data, int32_t real_packet_size, uint16_t header_size) -> void;
	private:
		auto get_version_and_size(unsigned char *header, uint16_t &version, uint16_t &size) -> void;
	};
}