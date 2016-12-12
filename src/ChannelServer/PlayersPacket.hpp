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

#include "PacketBuilder.hpp"
#include "SplitPacketBuilder.hpp"
#include "Types.hpp"
#include <string>

class PacketReader;
class Player;
struct Attack;
struct ReturnDamageInfo;
struct WidePoint;

namespace PlayersPacket {
	SPLIT_PACKET(showMoving, player_id_t playerId, unsigned char *buf, size_t size);
	SPLIT_PACKET(faceExpression, player_id_t playerId, int32_t face);
	PACKET(showChat, player_id_t playerId, bool isGm, const string_t &msg, bool bubbleOnly);
	SPLIT_PACKET(damagePlayer, player_id_t playerId, int32_t dmg, int32_t mob, uint8_t hit, int8_t type, uint8_t stance, int32_t noDamageSkill, const ReturnDamageInfo &pgmr);
	PACKET(showInfo, Player *getInfo, bool isSelf);
	PACKET(findPlayer, const string_t &name, int32_t map, uint8_t is = 0, bool isChannel = false);
	PACKET(whisperPlayer, const string_t &whispererName, channel_id_t channel, const string_t &message);
	SPLIT_PACKET(useMeleeAttack, player_id_t playerId, skill_id_t masterySkillId, skill_level_t masteryLevel, const Attack &attack);
	SPLIT_PACKET(useRangedAttack, player_id_t playerId, skill_id_t masterySkillId, skill_level_t masteryLevel, const Attack &attack);
	SPLIT_PACKET(useSpellAttack, player_id_t playerId, const Attack &attack);
	SPLIT_PACKET(useSummonAttack, player_id_t playerId, const Attack &attack);
	SPLIT_PACKET(useBombAttack, player_id_t playerId, charge_time_t chargeTime, skill_id_t skillId, const WidePoint &pos);
	SPLIT_PACKET(useEnergyChargeAttack, player_id_t playerId, skill_id_t masterySkillId, skill_level_t masteryLevel, const Attack &attack);
}