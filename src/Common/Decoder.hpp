/*
Copyright (C) 2008-2014 Vana Development Team

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

#include "BlockCipherIv.hpp"
#include "MapleVersion.hpp"
#include "Types.hpp"
#include <botan/pipe.h>
#include <botan/filters.h>
#include <botan/lookup.h>
#include <string>

class PacketCreator;

class Decoder {
public:
	explicit Decoder(bool encrypted);

	auto validPacket(unsigned char *header) -> bool;
	auto getLength(unsigned char *header) -> uint16_t;

	auto createHeader(unsigned char *header, uint16_t size) -> void;
	auto encrypt(unsigned char *buffer, int32_t size, uint16_t headerLen) -> void;
	auto decrypt(unsigned char *buffer, int32_t size, uint16_t headerLen) -> void;
	auto setRecvIv(uint32_t iv) -> void { m_recv.updateIv(iv); }
	auto setSendIv(uint32_t iv) -> void { m_send.updateIv(iv); }
	auto getRecvIv() const -> uint32_t { return m_recv.getIv(); }
	auto getSendIv() const -> uint32_t { return m_send.getIv(); }
private:
	auto isEncrypted() const -> bool { return m_encrypted; }
	auto getVersionAndSize(unsigned char *header, uint16_t &version, uint16_t &size) -> void;

	BlockCipherIv m_recv;
	BlockCipherIv m_send;
	bool m_encrypted = true;
	Botan::OctetString m_botanKey;
};

inline
auto Decoder::getLength(unsigned char *header) -> uint16_t {
	uint16_t version = 0;
	uint16_t pSize = 0;
	getVersionAndSize(header, version, pSize);
	return pSize;
}

inline
auto Decoder::validPacket(unsigned char *header) -> bool {
	uint16_t version = 0;
	uint16_t pSize = 0;
	getVersionAndSize(header, version, pSize);
	return (version == MapleVersion::Version && pSize >= 2);
}

inline
auto Decoder::getVersionAndSize(unsigned char *header, uint16_t &version, uint16_t &size) -> void {
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