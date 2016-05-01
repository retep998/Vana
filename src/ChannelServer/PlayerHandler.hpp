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

namespace Vana {
	class PacketReader;
	enum class SkillType;
	struct AttackData;

	namespace ChannelServer {
		class Player;

		namespace PlayerHandler {
			auto handleDoorUse(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleDamage(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleFacialExpression(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleGetInfo(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleHeal(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleMoving(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleSpecialSkills(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleMonsterBook(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto handleAdminMessenger(ref_ptr_t<Player> player, PacketReader &reader) -> void;

			auto useBombSkill(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useMeleeAttack(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useRangedAttack(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useSpellAttack(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useEnergyChargeAttack(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto useSummonAttack(ref_ptr_t<Player> player, PacketReader &reader) -> void;
			auto compileAttack(ref_ptr_t<Player> player, PacketReader &reader, SkillType skillType) -> AttackData;
		}
	}
}