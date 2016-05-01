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
#include "PacketTransformer.hpp"
#include "Common/BitUtilities.hpp"
#include "Common/CommonHeader.hpp"
#include "Common/MapleVersion.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/Randomizer.hpp"
#include <botan/filters.h>
#include <botan/lookup.h>
#include <botan/pipe.h>

namespace Vana {

auto PacketTransformer::testPacket(unsigned char *header) -> ValidityResult {
	uint16_t version = 0;
	uint16_t pSize = 0;
	getVersionAndSize(header, version, pSize);
	// All packets are at least a header
	return version == MapleVersion::Version && pSize >= sizeof(header_t) ?
		ValidityResult::Valid :
		ValidityResult::Invalid;
}

auto PacketTransformer::getPacketLength(unsigned char *header) -> uint16_t {
	uint16_t version = 0;
	uint16_t pSize = 0;
	getVersionAndSize(header, version, pSize);
	return pSize;
}

auto PacketTransformer::setPacketHeader(unsigned char *header, uint16_t realPacketSize) -> void {
	*reinterpret_cast<uint16_t *>(header) = MapleVersion::Version;
	*reinterpret_cast<uint16_t *>(header + sizeof(uint16_t)) = realPacketSize;
}

auto PacketTransformer::encryptPacket(unsigned char *packetData, int32_t realPacketSize, uint16_t headerSize) -> void {
	// Intentionally blank
}

auto PacketTransformer::decryptPacket(unsigned char *packetData, int32_t realPacketSize, uint16_t headerSize) -> void {
	// Intentionally blank
}

auto PacketTransformer::getVersionAndSize(unsigned char *header, uint16_t &version, uint16_t &size) -> void {
	version = *reinterpret_cast<uint16_t *>(header);
	size = *reinterpret_cast<uint16_t *>(header + sizeof(uint16_t));
}

}