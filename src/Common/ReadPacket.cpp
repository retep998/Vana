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
#include "ReadPacket.h"

ReadPacket::ReadPacket(unsigned char *buffer, size_t length) : buffer(buffer), length(length), pos(0) { }

unsigned char ReadPacket::getByte() {
	return buffer[pos++];
}

void ReadPacket::skipBytes(size_t len) {
	pos += len;
}

int ReadPacket::getInt() {
	int val = buffer[pos] + buffer[pos+1]*0x100 + buffer[pos+2]*0x10000 + buffer[pos+3]*0x1000000;
	pos += 4;
	return val;
}

short ReadPacket::getHeader() {
	return buffer[0] + buffer[1]*0x100;
}

short ReadPacket::getShort() {
	short val = buffer[pos] + buffer[pos+1]*0x100;;
	pos += 2;
	return val;
}

string ReadPacket::getString() {
	return getString(getShort());
}

string ReadPacket::getString(size_t len) {
	string s((char *) buffer + pos, len);
	pos += len;
	return s;
}

unsigned char * ReadPacket::getBuffer() {
	return buffer + pos;
}

size_t ReadPacket::getBufferLength() {
	return length - pos;
}

void ReadPacket::reset() {
	pos = 0;
}