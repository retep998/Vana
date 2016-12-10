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
#include "packet_transformer.hpp"
#include "common/common_header.hpp"
#include "common/maple_version.hpp"
#include "common/packet_builder.hpp"
#include "common/util/bit.hpp"
#include "common/util/randomizer.hpp"
#include <botan/filters.h>
#include <botan/lookup.h>
#include <botan/pipe.h>

namespace vana {

auto packet_transformer::test_packet(unsigned char *header) -> validity_result {
	uint16_t version = 0;
	uint16_t p_size = 0;
	get_version_and_size(header, version, p_size);
	// All packets are at least a header
	return version == maple_version::version && p_size >= sizeof(packet_header) ?
		validity_result::valid :
		validity_result::invalid;
}

auto packet_transformer::get_packet_length(unsigned char *header) -> uint16_t {
	uint16_t version = 0;
	uint16_t p_size = 0;
	get_version_and_size(header, version, p_size);
	return p_size;
}

auto packet_transformer::set_packet_header(unsigned char *header, uint16_t real_packet_size) -> void {
	*reinterpret_cast<uint16_t *>(header) = maple_version::version;
	*reinterpret_cast<uint16_t *>(header + sizeof(uint16_t)) = real_packet_size;
}

auto packet_transformer::encrypt_packet(unsigned char *packet_data, int32_t real_packet_size, uint16_t header_size) -> void {
	// Intentionally blank
}

auto packet_transformer::decrypt_packet(unsigned char *packet_data, int32_t real_packet_size, uint16_t header_size) -> void {
	// Intentionally blank
}

auto packet_transformer::get_version_and_size(unsigned char *header, uint16_t &version, uint16_t &size) -> void {
	version = *reinterpret_cast<uint16_t *>(header);
	size = *reinterpret_cast<uint16_t *>(header + sizeof(uint16_t));
}

}