/*
Copyright (C) 2008-2016 Vana Development Team

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


#include "Common/BlockCipherIv.hpp"
#include "Common/PacketTransformer.hpp"
#include "Common/Types.hpp"
#include <botan/lookup.h>

namespace Vana {
	class EncryptedPacketTransformer final : public PacketTransformer {
	public:
		EncryptedPacketTransformer(iv_t recvIv, iv_t sendIv);
		auto testPacket(unsigned char *header) -> ValidityResult override;
		auto getPacketLength(unsigned char *header) -> uint16_t override;
		auto setPacketHeader(unsigned char *header, uint16_t realPacketSize) -> void override;
		auto encryptPacket(unsigned char *packetData, int32_t realPacketSize, uint16_t headerSize) -> void override;
		auto decryptPacket(unsigned char *packetData, int32_t realPacketSize, uint16_t headerSize) -> void override;
	private:
		auto getVersionAndSize(unsigned char *header, uint16_t &version, uint16_t &size) -> void;

		BlockCipherIv m_recv;
		BlockCipherIv m_send;
		Botan::OctetString m_botanKey;
	};
}