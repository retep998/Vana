/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "FileTime.hpp"
#include "PacketBuilder.hpp"
#include "Types.hpp"
#include <vector>
#include <string>

namespace Vana {
	class ClientIp;
	class UserConnection;
	class World;
	struct Character;

	namespace Packets {
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

		PACKET(loginError, int16_t errorId);
		PACKET(loginBan, int8_t reason, FileTime expire);
		PACKET(loginProcess, int8_t id);
		PACKET(loginConnect, UserConnection *user, const string_t &username);
		PACKET(pinAssigned);
		PACKET(genderDone, gender_id_t gender);
		PACKET(showWorld, World *world);
		PACKET(worldEnd);
		PACKET(showChannels, int8_t status);
		PACKET(channelSelect);
		PACKET(channelOffline);
		PACKET(showAllCharactersInfo, world_id_t worldCount, uint32_t unk);
		PACKET(showViewAllCharacters, world_id_t worldId, const vector_t<Character> &chars);
		PACKET(showCharacters, const vector_t<Character> &chars, int32_t maxChars);
		PACKET(showCharacter, const Character &charc);
		PACKET(checkName, const string_t &name, uint8_t message);
		PACKET(deleteCharacter, player_id_t id, uint8_t result);
		PACKET(connectIp, const ClientIp &ip, port_t port, player_id_t charId);
		PACKET(relogResponse);
	}
}