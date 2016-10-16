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
#include "packet_reader.hpp"
#include "common/util/string.hpp"
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace vana {

packet_reader::packet_reader(unsigned char *buffer, size_t length) :
	m_buffer{buffer},
	m_length{length}
{
}

auto packet_reader::skip(int32_t len) -> packet_reader & {
	m_pos += len;
	return *this;
}

auto packet_reader::get_buffer() const -> unsigned char * {
	return m_buffer + m_pos;
}

auto packet_reader::get_buffer_length() const -> size_t {
	return get_size() - m_pos;
}

auto packet_reader::get_consumed_length() const -> size_t {
	return m_pos;
}

auto packet_reader::reset(int32_t len) -> packet_reader & {
	if (len >= 0) {
		m_pos = len;
	}
	else {
		m_pos = get_size() + len; // In this case, len is negative here so we take the total length and plus (minus) it by len
	}

	return *this;
}

auto packet_reader::to_string() const -> string {
	return vana::util::str::bytes_to_hex(get_buffer(), get_buffer_length());
}

}