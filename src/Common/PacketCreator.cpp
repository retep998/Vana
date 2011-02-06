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
#include "PacketCreator.h"
#include "Pos.h"
#include "PacketReader.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

PacketCreator::PacketCreator() : 
m_pos(0),
m_packet(new unsigned char[bufferLen]),
m_packetCapacity(bufferLen)
{
}

void PacketCreator::addPos(const Pos &pos) {
	add<int16_t>(pos.x);
	add<int16_t>(pos.y);
}

void PacketCreator::addBool(bool value) {
	add<int8_t>(value ? 1 : 0);
}

void PacketCreator::addBuffer(PacketCreator &packet) {
	addBuffer(packet.getBuffer(), packet.getSize());
}

void PacketCreator::addBuffer(PacketReader &packet) {
	addBuffer(packet.getBuffer(), packet.getBufferLength());
}

void PacketCreator::addBytes(const char *hex) {
	for (size_t i = 0; i < strlen(hex) / 2; i++) {
		unsigned char byte1 = hex[i * 2];
		unsigned char byte2 = hex[i * 2 + 1];
		if (byte1 >= 'A' && byte1 <= 'F')
			byte1 -= 'A' - 0xa;
		else if (byte1 >= 'a' && byte1 <= 'f')
			byte1 -= 'a' - 0xa;
		else if (byte1 >= '0' && byte1 <= '9')
			byte1 -= '0';
		if (byte2 >= 'A' && byte2 <= 'F')
			byte2 -= 'A' - 0xa;
		else if (byte2 >= 'a' && byte2 <= 'f')
			byte2 -= 'a' - 0xa;
		else if (byte2 >= '0' && byte2 <= '9')
			byte2 -= '0';
		unsigned char byte = byte1 * 0x10 + byte2;
		add<uint8_t>(byte);
	}
}

void PacketCreator::addString(const string &str, size_t len) {
	size_t slen = str.size();
	if (len < slen) {
		std::cout << "ERROR: addString used with length shorter than string size." << std::endl; // TODO: Throw exception
	}
	strncpy((char *) getBuffer(m_pos, len), str.c_str(), slen);
	for (size_t i = slen; i < len; i++) {
		m_packet[m_pos + i] = 0;
	}
	m_pos += len;
}

void PacketCreator::addString(const string &str) {
	size_t len = str.size();
	add<int16_t>(len);
	addString(str, str.size());
}

unsigned char * PacketCreator::getBuffer(size_t pos, size_t len) {
	if (m_packetCapacity < pos + len) { // Buffer is not large enough
		while (m_packetCapacity < pos + len) {
			m_packetCapacity *= 2; // Double the capacity each time the buffer is full
		}
		unsigned char *newBuffer = new unsigned char[m_packetCapacity];
		memcpy(newBuffer, m_packet.get(), pos);
		m_packet.reset(newBuffer);	
	}

	return m_packet.get() + pos;
}