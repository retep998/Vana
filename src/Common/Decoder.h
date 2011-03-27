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
#pragma once

#include "MapleEncryption.h"
#include "Types.h"
#include <string>

using std::string;

class PacketCreator;

class Decoder {
public:
	static int32_t getLength(unsigned char *header, bool encrypted);

	void createHeader(unsigned char *header, uint16_t size);

	PacketCreator getConnectPacket(const string &patchLocation = "");

	void encrypt(unsigned char *buffer, int32_t size);
	void decrypt(unsigned char *buffer, int32_t size);
	void setEncryption(bool encrypted) { m_encrypted = encrypted; }
	void setIvRecv(unsigned char *iv) { MapleEncryption::setIv(ivRecv, iv); }
	void setIvSend(unsigned char *iv) { MapleEncryption::setIv(ivSend, iv); }
private:
	bool isEncrypted() const { return m_encrypted; }

	unsigned char ivRecv[16];
	unsigned char ivSend[16];
	bool m_encrypted;
};

inline
int32_t Decoder::getLength(unsigned char *header, bool encrypted) {
	if (!encrypted) {
		// Only the bottom 2 bytes are interesting
		return (*(int16_t *)(header + 2));
	}
	return (header[0] | (header[1] << 8)) ^ (header[2] | (header[3] << 8));
}