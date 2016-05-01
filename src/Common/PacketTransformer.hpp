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

#include "Common/Types.hpp"

namespace Vana {
	class PacketTransformer {
	public:
		virtual auto testPacket(unsigned char *header) -> ValidityResult;
		virtual auto getPacketLength(unsigned char *header) -> uint16_t;
		virtual auto setPacketHeader(unsigned char *header, uint16_t realPacketSize) -> void;
		virtual auto encryptPacket(unsigned char *packetData, int32_t realPacketSize, uint16_t headerSize) -> void;
		virtual auto decryptPacket(unsigned char *packetData, int32_t realPacketSize, uint16_t headerSize) -> void;
	private:
		auto getVersionAndSize(unsigned char *header, uint16_t &version, uint16_t &size) -> void;
	};
}