/*
Copyright (C) 2008-2011 Vana Development Team

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
#include "PacketReader.h"
#include "Pos.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>

PacketReader::PacketReader() : m_length(0), m_pos(0) { }
PacketReader::PacketReader(unsigned char *buffer, size_t length) : m_buffer(buffer), m_length(length), m_pos(0) { }

void PacketReader::skipBytes(int32_t len) {
	m_pos += len;
}

int16_t PacketReader::getHeader(bool advanceBuffer) {
	if (getSize() < sizeof(header_t)) {
		throw std::range_error("Packet data longer than buffer allows");
	}
	if (advanceBuffer) {
		m_pos += sizeof(header_t);
	}
	return (*(header_t *)(m_buffer));
}

string PacketReader::getString() {
	return getString(get<uint16_t>());
}

string PacketReader::getString(size_t len) {
	if (len > getBufferLength()) {
		throw std::range_error("Packet string longer than buffer allows");
	}
	string s((char *) m_buffer + m_pos, len);
	m_pos += len;
	return s;
}

unsigned char * PacketReader::getBuffer() const {
	return m_buffer + m_pos;
}

size_t PacketReader::getBufferLength() const {
	return getSize() - m_pos;
}

PacketReader & PacketReader::reset(int32_t len) {
	if (len >= 0) {
		m_pos = len;
	}
	else {
		m_pos = getSize() + len; // In this case, len is negative here so we take the total length and plus (minus) it by len
	}

	return *this;
}

string PacketReader::toString() const {
	string ret;
	if (getBufferLength() > 0) {
		std::stringstream out;
		unsigned char *p = getBuffer();
		size_t buflen = getBufferLength() - 1;
		for (size_t i = 0; i <= buflen; i++) {
			out << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int16_t>(p[i]);
			if (i < buflen) {
				out << " ";
			}
		}
		ret = out.str();
	}
	return ret;
}

Pos PacketReader::getPos() {
	Pos ret;
	ret.x = get<int16_t>();
	ret.y = get<int16_t>();
	return ret;
}

bool PacketReader::getBool() {
	return (get<int8_t>() != 0);
}
