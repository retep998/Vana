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
	void useSkill(Player *player, int32_t skillid, int32_t time, ActiveBuff &pskill, ActiveMapBuff &mskill, int16_t addedinfo);
	void endSkill(Player *player, ActiveBuff &pskill);
	void giveDebuff(Player *player, uint8_t skillid, uint8_t level, int16_t time, int16_t delay, ActiveBuff &pskill, ActiveMapBuff &mskill);
	void endDebuff(Player *player, ActiveBuff &pskill);

	// Specific skills
	void usePirateBuff(Player *player, int32_t skillid, int32_t time, ActiveBuff &pskill, ActiveMapBuff &mskill);
	void useSpeedInfusion(Player *player, int32_t skillid, int32_t time, ActiveBuff &pskill, ActiveMapBuff &mskill, int16_t addedinfo);
	void useMount(Player *player, int32_t skillid, int32_t time, ActiveBuff &pskill, ActiveMapBuff &mskill, int16_t addedinfo, int32_t mountid);
	void useHomingBeacon(Player *player, int32_t skillid, ActiveBuff &pskill, int32_t mapmobid);
};
