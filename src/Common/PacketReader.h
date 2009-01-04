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
#ifndef PACKETREADER_H
#define PACKETREADER_H

#include "Types.h"
#include <boost/tr1/memory.hpp>
#include <string>

using std::string;

class PacketReader {
public:
	PacketReader(unsigned char *buffer, size_t length);

	unsigned char getByte();
	void skipBytes(int32_t len);
	int32_t getInt();
	int16_t getHeader(); // Basically getShort that always read at start
	int16_t getShort();
	string getString();
	string getString(size_t len);
	clock_t getClock();
	unsigned char * getBuffer();
	size_t getBufferLength();

	PacketReader & reset(int32_t len = 0);
private:
	unsigned char *buffer;
	size_t length;
	size_t pos;
};

#endif
