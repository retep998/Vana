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
struct ActiveBuff;
struct ActiveMapBuff;

namespace BuffsPacket {
	void useSkill(Player *player, int32_t skillId, int32_t time, ActiveBuff &playerSkill, ActiveMapBuff &mapSkill, int16_t addedInfo);
	void endSkill(Player *player, ActiveBuff &playerSkill);
	void giveDebuff(Player *player, uint8_t skillId, uint8_t level, int16_t time, int16_t delay, ActiveBuff &playerSkill, ActiveMapBuff &mapSkill);
	void endDebuff(Player *player, ActiveBuff &playerSkill);

	// Specific skills
	void usePirateBuff(Player *player, int32_t skillId, int32_t time, ActiveBuff &playerSkill, ActiveMapBuff &mapSkill);
	void useSpeedInfusion(Player *player, int32_t skillId, int32_t time, ActiveBuff &playerSkill, ActiveMapBuff &mapSkill, int16_t addedInfo);
	void useMount(Player *player, int32_t skillId, int32_t time, ActiveBuff &playerSkill, ActiveMapBuff &mapSkill, int16_t addedInfo, int32_t mountId);
	void useHomingBeacon(Player *player, int32_t skillId, ActiveBuff &playerSkill, int32_t mapMobId);
}