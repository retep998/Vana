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

#include "Types.hpp"
#include <string>
#include <vector>

class PacketCreator;
class Player;
struct Rates;

namespace WorldServerConnectPacket {
	auto findPlayer(int32_t playerId, const string_t &findeeName) -> void;
	auto whisperPlayer(int32_t playerId, const string_t &whisperee, const string_t &message) -> void;
	auto groupChat(int8_t type, int32_t playerId, const vector_t<int32_t> &receivers, const string_t &chat) -> void;
	auto rankingCalculation() -> void;
	auto sendToChannels(PacketCreator &packet) -> void;
	auto sendToWorlds(PacketCreator &packet) -> void;
	auto reloadMcdb(const string_t &type) -> void;
	auto rehashConfig() -> void;
}