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

class Player;
struct ActiveQuest;

namespace QuestsPacket {
	enum ErrorCodes {
		ErrorUnk = 0x09,
		ErrorNoItemSpace = 0x0A,
		ErrorNotEnoughMesos = 0x0B,
		ErrorEquipWorn = 0x0D,
		ErrorOnlyOne = 0x0E
	};
	void acceptQuest(Player *player, int16_t questid, int32_t npcid);
	void updateQuest(Player *player, const ActiveQuest &quest);
	void doneQuest(Player *player, int16_t questid);
	void questError(Player *player, int16_t questid, int8_t errorcode);
	void questExpire(Player *player, int16_t questid);
	void questFinish(Player *player, int16_t questid, int32_t npcid, int16_t nextquest, int64_t time);
	void forfeitQuest(Player *player, int16_t questid);
	void giveItem(Player *player, int32_t itemid, int32_t amount);
	void giveMesos(Player *player, int32_t amount);
	void giveFame(Player *player, int32_t amount);
};
