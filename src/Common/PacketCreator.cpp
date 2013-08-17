/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "PacketCreator.h"
#include "PacketReader.h"
#include "StringUtilities.h"
#include <cctype>
#include <iomanip>
#include <iostream>
#include <stdexcept>

PacketCreator::PacketCreator() :
	m_pos(0),
	m_packet(new unsigned char[bufferLen]),
	m_packetCapacity(bufferLen)
{
}

void PacketCreator::addBuffer(const PacketCreator &packet) {
	addBuffer(packet.getBuffer(), packet.getSize());
}

void PacketCreator::addBuffer(const PacketReader &packet) {
	addBuffer(packet.getBuffer(), packet.getBufferLength());
}

void PacketCreator::addBytes(const char *hex) {
	size_t x = 0;
	for (size_t i = 0; i < strlen(hex) / 2; ++i) {
		x = i * 2;
		unsigned char byte1 = getHexByte(hex[x]);
		unsigned char byte2 = getHexByte(hex[x + 1]);
		unsigned char byte = byte1 * 0x10 + byte2;
		add<uint8_t>(byte);
	}
}

unsigned char PacketCreator::getHexByte(unsigned char input) {
	input = static_cast<unsigned char>(toupper(input));
	if (input >= 'A' && input <= 'F') {
		input -= 'A' - 0xA;
	}
	else if (input >= '0' && input <= '9') {
		input -= '0';
	}
	else {
		throw std::invalid_argument("addBytes used with a non-hex digit");
	}
	return input;
}

void PacketCreator::addString(const string &str, size_t len) {
	size_t slen = str.size();
	if (len < slen) {
		throw std::invalid_argument("addString used with a length shorter than string size");
	}
	strncpy((char *) getBuffer(m_pos, len), str.c_str(), slen);
	for (size_t i = slen; i < len; i++) {
		m_packet[m_pos + i] = 0;
	}
	m_pos += len;
}

void PacketCreator::addString(const string &str) {
	uint16_t len = static_cast<uint16_t>(str.size());
	add<uint16_t>(len);
	addString(str, len);
}

unsigned char * PacketCreator::getBuffer(size_t pos, size_t len) {
	if (m_packetCapacity < pos + len) {
		// Buffer is not large enough
		while (m_packetCapacity < pos + len) {
			m_packetCapacity *= 2; // Double the capacity each time the buffer is full
		}
		unsigned char *newBuffer = new unsigned char[m_packetCapacity];
		memcpy(newBuffer, m_packet.get(), pos);
		m_packet.reset(newBuffer);
	}

	return m_packet.get() + pos;
}

string PacketCreator::toString() const {
	return StringUtilities::bytesToHex(getBuffer(), getSize());
}