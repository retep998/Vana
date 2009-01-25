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
#ifndef BUFFSPACK_H
#define BUFFSPACK_H

#include "Types.h"

class Player;
struct SkillActiveInfo;

namespace BuffsPacket {
	void useDash(Player *player, int32_t time, SkillActiveInfo pskill);
	void useSkill(Player *player, int32_t skillid, int32_t time, SkillActiveInfo pskill, int16_t addedinfo, bool ismorph = false, bool isitem = false);
	void useSpeedInfusion(Player *player, int32_t time, SkillActiveInfo pskill, int16_t addedinfo);
	void useMount(Player *player, int32_t skillid, int32_t time, SkillActiveInfo pskill, int16_t addedinfo, int32_t mountid);
	void endSkill(Player *player, SkillActiveInfo pskill);
};

#endif