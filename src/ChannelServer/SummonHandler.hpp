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

#include "Common/Types.hpp"
#include <string>

namespace Vana {
	class BuffInfo;
	class PacketReader;
	struct SkillLevelInfo;
	template <typename T> class IdPool;

	namespace ChannelServer {
		class Player;
		struct BuffPacketValues;

		namespace SummonMessages {
			enum Messages : int8_t {
				OutOfTime = 0x00,
				Disappearing = 0x03,
				None = 0x04
			};
		}

		namespace SummonHandler {
			extern IdPool<summon_id_t> summonIds;
			auto useSummon(ref_ptr_t<Player> player, skill_id_t skillId, skill_level_t level) -> void;
			auto removeSummon(ref_ptr_t<Player> player, summon_id_t summonId, bool packetOnly, int8_t showMessage, bool fromTimer = false) -> void;
			auto showSummon(ref_ptr_t<Player> player) -> void;
			auto showSummons(ref_ptr_t<Player> fromPlayer, ref_ptr_t<Player> toPlayer) -> void;
			auto moveSummon(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto damageSummon(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto makeBuff(ref_ptr_t<Player> player, item_id_t itemId) -> BuffInfo;
			auto makeActiveBuff(ref_ptr_t<Player> player, const BuffInfo &data, item_id_t itemId, const SkillLevelInfo *skillInfo) -> BuffPacketValues;
			auto summonSkill(ref_ptr_t<Player> player, PacketReader &reader) -> void;
		}
	}
}