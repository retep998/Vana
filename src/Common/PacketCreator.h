/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef PACKETCREATOR_H
#define PACKETCREATOR_H

#include "Types.h"
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;

class PacketReader;
struct Pos;

class PacketCreator {
public:
	PacketCreator() : pos(0) { }

	void addInt(int32_t intg);
	void setInt(int32_t intg, size_t pos);
	void addInt64(int64_t int64);
	void setInt64(int64_t int64, size_t pos);
	void addShort(int16_t shrt);
	void setShort(int16_t shrt, size_t pos);
	void addString(const string &str); // Dynamically-lengthed strings
	void addString(const string &str, size_t len); // Static-lengthed strings
	void addPos(Pos pos); // Positions
	void addByte(unsigned char byte);
	void setByte(unsigned char byte, size_t pos);
	void addBytes(const char *hex);
	void addClock(clock_t clock);
	void addBuffer(unsigned char *bytes, size_t len);
	void addBuffer(PacketReader &packet);
	void addIP(const string &ip);

	const unsigned char * getBuffer() const;
	size_t getSize() const;
private:
	static const size_t bufferLen = 10000;

	size_t pos;
	unsigned char packet[bufferLen];
};

inline
void PacketCreator::addInt(int32_t intg) {
	(*(int32_t*)(packet + pos)) = intg;
	pos += 4;
}

inline
void PacketCreator::setInt(int32_t intg, size_t pos) {
	(*(int32_t*)(packet + pos)) = intg;
}

inline
void PacketCreator::addInt64(int64_t int64) {
	(*(int64_t*)(packet + pos)) = int64;
	pos += 8;
}

inline
void PacketCreator::setInt64(int64_t int64, size_t pos) {
	(*(int64_t*)(packet + pos)) = int64;
}

inline
void PacketCreator::addShort(int16_t shrt) {
	(*(int16_t*)(packet + pos)) = shrt;
	pos += 2;
}

inline
void PacketCreator::setShort(int16_t shrt, size_t pos) {
	(*(int16_t*)(packet + pos)) = shrt;
}

inline
void PacketCreator::addByte(unsigned char byte) {
	packet[pos++] = byte;
}

inline
void PacketCreator::setByte(unsigned char byte, size_t pos) {
	packet[pos] = byte;
}

inline
void PacketCreator::addClock(clock_t clock) {
	packet[pos++] = sizeof(clock_t);
	(*(clock_t*)(packet + pos)) = clock;
	pos += sizeof(clock_t);
}

inline
const unsigned char * PacketCreator::getBuffer() const {
	return packet;
}

inline
size_t PacketCreator::getSize() const {
	return pos;
}

#endif
