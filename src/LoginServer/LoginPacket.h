/*
Copyright (C) 2008 Vana Development Team

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

#include <vector>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

class PlayerLogin;
struct World;
struct Character;

namespace LoginPacket {
	void loginError(PlayerLogin* player, short errorid);
	void loginBan(PlayerLogin* player, char reason, int expire);
	void loginProcess(PlayerLogin* player, char id);
	void loginConnect(PlayerLogin* player, char* username, int size);
	void pinAssigned(PlayerLogin* player);
	void genderDone(PlayerLogin* player, char gender);
	void showWorld(PlayerLogin* player, World *world);
	void worldEnd(PlayerLogin* player);
	void showChannels(PlayerLogin* player);
	void channelSelect(PlayerLogin* player);
	void showCharacters(PlayerLogin* player, vector <Character> chars);
	void showCharacter(PlayerLogin* player, Character charc);
	void checkName(PlayerLogin* player, char is, char* name);
	void deleteCharacter(PlayerLogin* player, int ID);
	void connectIP(PlayerLogin* player, int charid);
	void logBack(PlayerLogin* player);
};

#endif