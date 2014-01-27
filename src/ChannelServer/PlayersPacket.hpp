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

#include "Types.hpp"
#include <string>

class PacketReader;
class Player;
struct Attack;
struct ReturnDamageInfo;

namespace PlayersPacket {
	auto showMoving(Player *player, unsigned char *buf, size_t size) -> void;
	auto faceExpression(Player *player, int32_t face) -> void;
	auto showChat(Player *player, const string_t &msg, bool bubbleOnly) -> void;
	auto damagePlayer(Player *player, int32_t dmg, int32_t mob, uint8_t hit, int8_t type, uint8_t stance, int32_t noDamageSkill, const ReturnDamageInfo &pgmr) -> void;
	auto showInfo(Player *player, Player *getInfo, bool isSelf) -> void;
	auto findPlayer(Player *player, const string_t &name, int32_t map, uint8_t is = 0, bool isChannel = false) -> void;
	auto whisperPlayer(Player *target, const string_t &whispererName, uint16_t channel, const string_t &message) -> void;
	auto sendToPlayers(unsigned char *data, int32_t len) -> void;
	auto useMeleeAttack(Player *player, const Attack &attack) -> void;
	auto useRangedAttack(Player *player, const Attack &attack) -> void;
	auto useSpellAttack(Player *player, const Attack &attack) -> void;
	auto useSummonAttack(Player *player, const Attack &attack) -> void;
	auto useEnergyChargeAttack(Player *player, const Attack &attack) -> void;
}