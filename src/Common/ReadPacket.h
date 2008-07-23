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
#ifndef READPACKET_H
#define READPACKET_H

#include <memory>
#include <string>

using std::string;

class ReadPacket {
public:
	ReadPacket(unsigned char *buffer, size_t length);
	
	unsigned char getByte();
	void skipBytes(int len);
	int getInt();
	short getHeader(); // Basically getShort that always read at start
	short getShort();
	string getString();
	string getString(size_t len);
	unsigned char * getBuffer();
	size_t getBufferLength();

	void reset(int len = 0);
private:
	unsigned char *buffer;
	size_t length;
	size_t pos;
};

#endif
