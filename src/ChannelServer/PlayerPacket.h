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
#ifndef PLAYERPACK_H
#define PLAYERPACK_H

#include <string>

using std::string;

class Player;
class KeyMaps;
class SkillMacros;

namespace PlayerPacket {
	void connectData(Player *player);
	void showKeys(Player *player, KeyMaps *keymaps);
	void showSkillMacros(Player *player, SkillMacros *macros);
	void updateStat(Player *player, int id, int value, bool is = false);
	void updateStat(Player *player, int id, short value, bool is = false);
	void updateStat(Player *player, int id, char value, bool is = false);
	void changeChannel(Player *player, const string &ip, short port);
	void showMessage(Player *player, const string &msg, char type);
	void instructionBubble(Player *player, const string &msg, short width = -1, short height = 5);
};

#endif
