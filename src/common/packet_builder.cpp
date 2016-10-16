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
#include "packet_builder.hpp"
#include "common/packet_reader.hpp"
#include "common/split_packet_builder.hpp"
#include "common/util/string.hpp"
#include <cctype>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace vana {

packet_builder::packet_builder() :
	m_packet{new unsigned char[default_buffer_len]},
	m_packet_capacity{default_buffer_len}
{
}

auto packet_builder::unk(int32_t bytes) -> packet_builder & {
	if (bytes <= 0) throw std::invalid_argument{"bytes must be > 0"};

	const size_t align_size = sizeof(uint32_t);
	size_t casted = static_cast<size_t>(bytes);
	size_t aligned_max = casted / align_size;
	size_t remainder = casted % align_size;
	for (size_t i = 0; i < aligned_max; i++) {
		unk<uint32_t>(0);
	}
	if (remainder != 0) {
		for (size_t i = 0; i < remainder; i++) {
			unk<uint8_t>(0);
		}
	}
	return *this;
}

auto packet_builder::add_bytes(const string &hex) -> packet_builder & {
	return add_bytes(hex.c_str());
}

auto packet_builder::add_bytes(const char *hex) -> packet_builder & {
	size_t x = 0;
	for (size_t i = 0; i < strlen(hex) / 2; ++i) {
		x = i * 2;
		unsigned char byte1 = get_hex_byte(hex[x]);
		unsigned char byte2 = get_hex_byte(hex[x + 1]);
		unsigned char byte = byte1 * 0x10 + byte2;
		add<uint8_t>(byte);
	}
	return *this;
}

auto packet_builder::get_hex_byte(unsigned char input) -> unsigned char {
	input = static_cast<unsigned char>(toupper(input));
	if (input >= 'A' && input <= 'F') {
		input -= 'A' - 0xA;
	}
	else if (input >= '0' && input <= '9') {
		input -= '0';
	}
	else {
		throw std::invalid_argument{"add_bytes used with a non-hex digit"};
	}
	return input;
}

auto packet_builder::add_buffer(const packet_builder &builder) -> packet_builder & {
	return add_buffer(builder.get_buffer(), builder.get_size());
}

auto packet_builder::add_buffer(const packet_reader &reader) -> packet_builder & {
	return add_buffer(reader.get_buffer(), reader.get_buffer_length());
}

auto packet_builder::get_buffer(size_t pos, size_t len) -> unsigned char * {
	if (m_packet_capacity < pos + len) {
		// Buffer is not large enough
		while (m_packet_capacity < pos + len) {
			m_packet_capacity *= 2; // Double the capacity each time the buffer is full
		}
		unsigned char *new_buffer = new unsigned char[m_packet_capacity];
		memcpy(new_buffer, m_packet.get(), pos);
		m_packet.reset(new_buffer);
	}

	return m_packet.get() + pos;
}

auto packet_builder::to_string() const -> string {
	return vana::util::str::bytes_to_hex(get_buffer(), get_size());
}

}