/*
Copyright (C) 2008 Vana Development Team

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

PacketReader::PacketReader(unsigned char *buffer, size_t length) : buffer(buffer), length(length), pos(0) { }

unsigned char PacketReader::getByte() {
	return buffer[pos++];
}

void PacketReader::skipBytes(int32_t len) {
	pos += len;
}

int32_t PacketReader::getInt() {
	int32_t val = buffer[pos] + buffer[pos + 1] * 0x100 + buffer[pos + 2] * 0x10000 + buffer[pos + 3] * 0x1000000;
	pos += 4;
	return val;
}

clock_t PacketReader::getClock() {
	clock_t val = 0;
	for (uint8_t i = 0; i < sizeof(clock_t); i++) {
		int32_t mul = 1;
		for (int8_t f = 0; f < i; f++) {
			mul *= 100;
		}
		val += buffer[pos + i] * mul;
	}
	pos += sizeof(clock_t);
	return val;
}

int16_t PacketReader::getHeader() {
	return buffer[0] + buffer[1] * 0x100;
}

int16_t PacketReader::getShort() {
	int16_t val = buffer[pos] + buffer[pos + 1] * 0x100;
	pos += 2;
	return val;
}

string PacketReader::getString() {
	return getString(getShort());
}

string PacketReader::getString(size_t len) {
	string s((char *) buffer + pos, len);
	pos += len;
	return s;
}

unsigned char * PacketReader::getBuffer() {
	return buffer + pos;
}

size_t PacketReader::getBufferLength() {
	return length - pos;
}

PacketReader & PacketReader::reset(int32_t len) {
	if (len >= 0) {
		pos = len;
	}
	else {
		pos = length + len; // In this case, len is negative here so we take the total length and plus (minus) it by len
	}

	return *this;
}
