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

class Player;

namespace PlayerPacket {
	void connectData(Player* player);
	void showKeys(Player* player, int keys[90]);
	void updateStat(Player *player, int id, int value, bool is = 0);
	void updateStat(Player *player, int id, short value, bool is = 0);
	void newHair(Player* player);
	void newEyes(Player* player);
	void newSkin(Player* player);
	void changeChannel(Player *player, char *ip, short port);
	void showMessage(Player *player, std::string msg, char type);
};

#endif
