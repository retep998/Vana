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
#include "PacketReader.hpp"
#include "StringUtilities.hpp"
#include <iomanip>
#include <sstream>
#include <stdexcept>

PacketReader::PacketReader(unsigned char *buffer, size_t length) :
	m_buffer(buffer),
	m_length(length)
{
}

auto PacketReader::skipBytes(int32_t len) -> void {
	m_pos += len;
}

auto PacketReader::getBuffer() const -> unsigned char * {
	return m_buffer + m_pos;
}

auto PacketReader::getBufferLength() const -> size_t {
	return getSize() - m_pos;
}

auto PacketReader::getConsumedLength() const -> size_t {
	return m_pos;
}

auto PacketReader::reset(int32_t len) -> PacketReader & {
	if (len >= 0) {
		m_pos = len;
	}
	else {
		m_pos = getSize() + len; // In this case, len is negative here so we take the total length and plus (minus) it by len
	}

	return *this;
}

auto PacketReader::toString() const -> string_t {
	return StringUtilities::bytesToHex(getBuffer(), getBufferLength());
}