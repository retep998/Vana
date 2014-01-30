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
#include <vector>
#include <string>

class Player;
class World;
struct Character;

namespace LoginPacket {
	namespace CheckNameErrors {
		enum Errors : uint8_t {
			None = 0x00,
			Taken = 0x01,
			Invalid = 0x04,
			UnknownReason = 0xFF
		};
	}
	namespace Errors {
		enum Errors : uint8_t {
			InvalidPin = 0x02,
			InvalidPassword = 0x04,
			InvalidUsername = 0x05,
			AlreadyLoggedIn = 0x07
		};
	}
	namespace WorldMessages {
		enum Messages : uint8_t {
			Normal = 0x00,
			HeavyLoad = 0x01,
			MaxLoad = 0x02
		};
	}
	auto loginError(Player *player, int16_t errorId) -> void;
	auto loginBan(Player *player, int8_t reason, int32_t expire) -> void;
	auto loginProcess(Player *player, int8_t id) -> void;
	auto loginConnect(Player *player, const string_t &username) -> void;
	auto pinAssigned(Player *player) -> void;
	auto genderDone(Player *player, int8_t gender) -> void;
	auto showWorld(Player *player, World *world) -> void;
	auto worldEnd(Player *player) -> void;
	auto showChannels(Player *player, int8_t status) -> void;
	auto channelSelect(Player *player) -> void;
	auto channelOffline(Player *player) -> void;
	auto showAllCharactersInfo(Player *player, world_id_t worldCount, uint32_t unk) -> void;
	auto showViewAllCharacters(Player *player, world_id_t worldId, const vector_t<Character> &chars) -> void;
	auto showCharacters(Player *player, const vector_t<Character> &chars, int32_t maxChars) -> void;
	auto showCharacter(Player *player, const Character &charc) -> void;
	auto checkName(Player *player, const string_t &name, uint8_t message) -> void;
	auto deleteCharacter(Player *player, int32_t id, uint8_t result) -> void;
	auto connectIp(Player *player, int32_t charId) -> void;
	auto relogResponse(Player *player) -> void;
}