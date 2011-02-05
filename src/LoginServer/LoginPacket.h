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

#include "Types.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

class Player;
class World;
struct Character;

namespace LoginPacket {
	namespace Errors {
		enum Errors {
			InvalidPin = 0x02,
			InvalidPassword = 0x04,
			InvalidUsername = 0x05,
			AlreadyLoggedIn = 0x07
		};
	}
	namespace WorldMessages {
		enum Messages {
			Normal = 0x00,
			HeavyLoad = 0x01,
			MaxLoad = 0x02
		};
	}
	void loginError(Player *player, int16_t errorid);
	void loginBan(Player *player, int8_t reason, int32_t expire);
	void loginProcess(Player *player, int8_t id);
	void loginConnect(Player *player, const string &username);
	void pinAssigned(Player *player);
	void genderDone(Player *player, int8_t gender);
	void showWorld(Player *player, World *world);
	void worldEnd(Player *player);
	void showChannels(Player *player, int8_t status);
	void channelSelect(Player *player);
	void channelOffline(Player *player);
	void showAllCharactersInfo(Player *player, uint32_t worlds, uint32_t unk);
	void showCharactersWorld(Player *player, uint8_t worldid, const vector<Character> &chars); // Used for "view all characters"
	void showCharacters(Player *player, const vector<Character> &chars, int32_t maxchars);
	void showCharacter(Player *player, const Character &charc);
	void checkName(Player *player, const string &name, bool taken);
	void deleteCharacter(Player *player, int32_t id, uint8_t result);
	void connectIp(Player *player, int32_t charid);
	void relogResponse(Player *player);
};
