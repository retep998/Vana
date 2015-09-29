/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Common/MobConstants.hpp"
#include "Common/PacketBuilder.hpp"
#include "Common/Types.hpp"
#include <vector>

namespace Vana {
	struct Point;

	namespace ChannelServer {
		class Mob;
		class Player;
		struct StatusInfo;

		namespace Packets {
			namespace Mobs {
				PACKET(spawnMob, ref_ptr_t<Mob> mob, int8_t summonEffect, ref_ptr_t<Mob> owner = nullptr, MobSpawnType spawn = MobSpawnType::Existing);
				PACKET(requestControl, ref_ptr_t<Mob> mob, MobSpawnType spawn);
				PACKET(mobPacket, ref_ptr_t<Mob> mob, int8_t summonEffect, ref_ptr_t<Mob> owner, MobSpawnType spawn);
				PACKET(endControlMob, map_object_t mapMobId);
				PACKET(moveMobResponse, map_object_t mapMobId, int16_t moveId, bool skillPossible, int32_t mp, mob_skill_id_t skill, mob_skill_level_t level);
				PACKET(moveMob, map_object_t mapMobId, bool skillPossible, int8_t rawAction, mob_skill_id_t skill, mob_skill_level_t level, int16_t option, unsigned char *buf, int32_t len);
				PACKET(healMob, map_object_t mapMobId, int32_t amount);
				PACKET(hurtMob, map_object_t mapMobId, damage_t amount);
				PACKET(damageFriendlyMob, ref_ptr_t<Mob> mob, damage_t damage);
				PACKET(applyStatus, map_object_t mapMobId, int32_t statusMask, const vector_t<StatusInfo> &info, int16_t delay, const vector_t<int32_t> &reflection);
				PACKET(removeStatus, map_object_t mapMobId, int32_t status);
				PACKET(showHp, map_object_t mapMobId, int8_t percentage);
				PACKET(showBossHp, ref_ptr_t<Mob> mob);
				PACKET(dieMob, map_object_t mapMobId, int8_t death = 1);
				PACKET(showSpawnEffect, int8_t summonEffect, const Point &pos);
			}
		}
	}
}