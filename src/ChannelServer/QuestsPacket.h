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
#ifndef QUESTSPACK_H
#define QUESTSPACK_H

#include "Types.h"

class Player;
struct Quest;

namespace QuestsPacket {
	void acceptQuest(Player *player, int16_t questid, int32_t npcid);
	void updateQuest(Player *player, const Quest &quest);
	void doneQuest(Player *player, int16_t questid);
	void questFinish(Player *player, int16_t questid, int32_t npcid, int16_t nextquest, int64_t time);
	void giveItem(Player *player, int32_t itemid, int32_t amount);
	void giveMesos(Player *player, int32_t amount);
};

#endif
