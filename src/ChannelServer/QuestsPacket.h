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
#ifndef QUESTSPACK_H
#define QUESTSPACK_H

#include <vector>

using std::vector;

class Player;

struct Quest;

namespace QuestsPacket {
	void acceptQuest(Player *player, short questid, int npcid);
	void updateQuest(Player *player, Quest quest);
	void doneQuest(Player *player, int questid);
	void questFinish(Player *player, short questid, int npcid, short nextquest, __int64 time);
	void giveItem(Player *player, int itemid, int amount);
	void giveMesos(Player *player, int amount);
};

#endif
