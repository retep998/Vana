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

#include "Ip.hpp"
#include "shared_array.hpp"
#include "Types.hpp"
#include <memory>
#include <unordered_map>

class PacketReader;

struct ConnectingPlayer {
	ConnectingPlayer() : connectIp(0) { }

	Ip connectIp;
	time_point_t connectTime;
	uint16_t packetSize;
	MiscUtilities::shared_array<unsigned char> heldPacket;
};

class Connectable {
	SINGLETON(Connectable);
public:
	auto newPlayer(int32_t id, const Ip &ip, PacketReader &reader) -> void;
	auto checkPlayer(int32_t id, const Ip &ip, bool &hasPacket) const -> Result;
	auto getPacket(int32_t id) const -> PacketReader;
	auto playerEstablished(int32_t id) -> void;
private:
	const static uint32_t MaxMilliseconds = 5000;

	hash_map_t<int32_t, ConnectingPlayer> m_map;
};