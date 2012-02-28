/*
Copyright (C) 2008-2012 Vana Development Team

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

#include "BlockCipherIv.h"
#include "MapleVersion.h"
#include "Types.h"
#include <string>

using std::string;

class PacketCreator;

class Decoder {
public:
	Decoder(bool encrypted);

	bool validPacket(unsigned char *header);
	uint16_t getLength(unsigned char *header);

	void createHeader(unsigned char *header, uint16_t size);

	PacketCreator getConnectPacket(const string &patchLocation = "");

	void encrypt(unsigned char *buffer, int32_t size);
	void decrypt(unsigned char *buffer, int32_t size);
	void setRecvIv(uint32_t iv) { m_recv.updateIv(iv); }
	void setSendIv(uint32_t iv) { m_send.updateIv(iv); }
private:
	bool isEncrypted() const { return m_encrypted; }
	void getVersionAndSize(unsigned char *header, uint16_t &version, uint16_t &size);

	BlockCipherIv m_recv;
	BlockCipherIv m_send;
	bool m_encrypted;
};

inline
uint16_t Decoder::getLength(unsigned char *header) {
	uint16_t version = 0;
	uint16_t pSize = 0;
	getVersionAndSize(header, version, pSize);
	return pSize;
}

inline
bool Decoder::validPacket(unsigned char *header) {
	uint16_t version = 0;
	uint16_t pSize = 0;
	getVersionAndSize(header, version, pSize);
	return (version == MapleVersion::Version && pSize >= 2);
}

inline
void Decoder::getVersionAndSize(unsigned char *header, uint16_t &version, uint16_t &size) {
	if (!isEncrypted()) {
		version = (*(uint16_t *)(header));
		size = (*(uint16_t *)(header + 2));
	}
	else {
		unsigned char *iv = m_recv.getBytes();
		uint16_t enc = ((iv[3] << 8) | iv[2]);
		version = (-(*(uint16_t *)(header)) - 1) ^ enc;
		size = (*(uint16_t *)(header)) ^ (*(uint16_t *)(header + 2));
	}
}