/*
Copyright (C) 2008-2009 Vana Development Team

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
#ifndef LOGINPACKET_H
#define LOGINPACKET_H

#define MAX_FIELD_SIZE 15

#include "Types.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

class PlayerLogin;
struct World;
struct Character;

namespace LoginPacket {
	void loginError(PlayerLogin *player, int16_t errorid);
	void loginBan(PlayerLogin *player, int8_t reason, int32_t expire);
	void loginProcess(PlayerLogin *player, int8_t id);
	void loginConnect(PlayerLogin *player, const string &username);
	void pinAssigned(PlayerLogin *player);
	void genderDone(PlayerLogin *player, int8_t gender);
	void showWorld(PlayerLogin *player, World *world);
	void worldEnd(PlayerLogin *player);
	void showChannels(PlayerLogin *player, int8_t status);
	void channelSelect(PlayerLogin *player);
	void channelOffline(PlayerLogin *player);
	void showAllCharactersInfo(PlayerLogin *player, uint32_t worlds, uint32_t unk);
	void showCharactersWorld(PlayerLogin *player, uint8_t worldid, const vector<Character> &chars); // Used for "view all characters"
	void showCharacters(PlayerLogin *player, const vector<Character> &chars, int32_t maxchars);
	void showCharacter(PlayerLogin *player, const Character &charc);
	void checkName(PlayerLogin *player, const string &name, bool taken);
	void deleteCharacter(PlayerLogin *player, int32_t id, bool success);
	void connectIp(PlayerLogin *player, int32_t charid);
	void relogResponse(PlayerLogin *player);
};

#endif
