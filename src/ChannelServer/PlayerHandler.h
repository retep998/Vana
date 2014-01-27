/*
Copyright (C) 2008-2014 Vana Development Team

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

class PacketReader;
class Player;
struct Attack;
struct MpEaterInfo;

namespace PlayerHandler {
	auto handleDoorUse(Player *player, PacketReader &packet) -> void;
	auto handleDamage(Player *player, PacketReader &packet) -> void;
	auto handleFacialExpression(Player *player, PacketReader &packet) -> void;
	auto handleGetInfo(Player *player, PacketReader &packet) -> void;
	auto handleHeal(Player *player, PacketReader &packet) -> void;
	auto handleMoving(Player *player, PacketReader &packet) -> void;
	auto handleSpecialSkills(Player *player, PacketReader &packet) -> void;
	auto handleMonsterBook(Player *player, PacketReader &packet) -> void;
	auto handleAdminMessenger(Player *player, PacketReader &packet) -> void;

	auto useMeleeAttack(Player *player, PacketReader &packet) -> void;
	auto useRangedAttack(Player *player, PacketReader &packet) -> void;
	auto useSpellAttack(Player *player, PacketReader &packet) -> void;
	auto useEnergyChargeAttack(Player *player, PacketReader &packet) -> void;
	auto useSummonAttack(Player *player, PacketReader &packet) -> void;
	auto compileAttack(Player *player, PacketReader &packet, int8_t skillType) -> Attack;
}