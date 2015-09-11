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

#include "Types.hpp"

namespace Vana {
	enum class SkillType;
	class PacketReader;
	class Player;
	struct AttackData;

	namespace PlayerHandler {
		auto handleDoorUse(Player *player, PacketReader &reader) -> void;
		auto handleDamage(Player *player, PacketReader &reader) -> void;
		auto handleFacialExpression(Player *player, PacketReader &reader) -> void;
		auto handleGetInfo(Player *player, PacketReader &reader) -> void;
		auto handleHeal(Player *player, PacketReader &reader) -> void;
		auto handleMoving(Player *player, PacketReader &reader) -> void;
		auto handleSpecialSkills(Player *player, PacketReader &reader) -> void;
		auto handleMonsterBook(Player *player, PacketReader &reader) -> void;
		auto handleAdminMessenger(Player *player, PacketReader &reader) -> void;

		auto useBombSkill(Player *player, PacketReader &reader) -> void;
		auto useMeleeAttack(Player *player, PacketReader &reader) -> void;
		auto useRangedAttack(Player *player, PacketReader &reader) -> void;
		auto useSpellAttack(Player *player, PacketReader &reader) -> void;
		auto useEnergyChargeAttack(Player *player, PacketReader &reader) -> void;
		auto useSummonAttack(Player *player, PacketReader &reader) -> void;
		auto compileAttack(Player *player, PacketReader &reader, SkillType skillType) -> AttackData;
	}
}