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
#ifndef DECODER_H
#define DECODER_H

#include "../SendHeader.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <string>

using std::string;

class Packet;

class Decoder {
private:
	unsigned char ivRecv[4];
	unsigned char ivSend[4];

public:
	static const int CONNECT_LENGTH = 15;
	static int getLength (unsigned char *header) {
		return ((header[0] + header[1]*0x100) ^ (header[2] + header[3]*0x100)); 
	}
	void createHeader (unsigned char *header, short size); 

	Packet & getConnectPacket(string unknown = "");

	void setIvRecv(unsigned char *iv) { memcpy_s(ivRecv, 4, iv, 4); }
	void setIvSend(unsigned char *iv) { memcpy_s(ivSend, 4, iv, 4); }

	void encrypt (unsigned char *buffer, int size);
	void decrypt (unsigned char *buffer, int size);
	void next ();
	static void Init();

};

#endif
