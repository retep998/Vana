/*
Copyright (C) 2008-2015 Vana Development Team

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
#include "PacketBuilder.hpp"
#include "PacketReader.hpp"
#include "SplitPacketBuilder.hpp"
#include "StringUtilities.hpp"
#include <cctype>
#include <iomanip>
#include <iostream>
#include <stdexcept>

PacketBuilder::PacketBuilder() :
	m_packet{new unsigned char[bufferLen]},
	m_packetCapacity{bufferLen}
{
}

auto PacketBuilder::addBytes(const string_t &hex) -> PacketBuilder & {
	return addBytes(hex.c_str());
}

auto PacketBuilder::addBytes(const char *hex) -> PacketBuilder & {
	size_t x = 0;
	for (size_t i = 0; i < strlen(hex) / 2; ++i) {
		x = i * 2;
		unsigned char byte1 = getHexByte(hex[x]);
		unsigned char byte2 = getHexByte(hex[x + 1]);
		unsigned char byte = byte1 * 0x10 + byte2;
		add<uint8_t>(byte);
	}
	return *this;
}

auto PacketBuilder::getHexByte(unsigned char input) -> unsigned char {
	input = static_cast<unsigned char>(toupper(input));
	if (input >= 'A' && input <= 'F') {
		input -= 'A' - 0xA;
	}
	else if (input >= '0' && input <= '9') {
		input -= '0';
	}
	else {
		throw std::invalid_argument{"addBytes used with a non-hex digit"};
	}
	return input;
}

auto PacketBuilder::addBuffer(const PacketBuilder &builder) -> PacketBuilder & {
	return addBuffer(builder.getBuffer(), builder.getSize());
}

auto PacketBuilder::addBuffer(const PacketReader &reader) -> PacketBuilder & {
	return addBuffer(reader.getBuffer(), reader.getBufferLength());
}

auto PacketBuilder::getBuffer(size_t pos, size_t len) -> unsigned char * {
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

auto PacketBuilder::toString() const -> string_t {
	return StringUtilities::bytesToHex(getBuffer(), getSize());
}