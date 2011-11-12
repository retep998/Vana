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
#include <string>

using std::string;

class PacketReader;
class Player;
struct Attack;
struct ReturnDamageInfo;

namespace PlayersPacket {
	void showMoving(Player *player, unsigned char *buf, size_t size);
	void faceExpression(Player *player, int32_t face);
	void showChat(Player *player, const string &msg, bool bubbleOnly);
	void damagePlayer(Player *player, int32_t dmg, int32_t mob, uint8_t hit, int8_t type, uint8_t stance, int32_t nodamageskill, const ReturnDamageInfo &pgmr);
	void showInfo(Player *player, Player *getinfo, bool isself);
	void findPlayer(Player *player, const string &name, int32_t map, uint8_t is = 0, bool isChannel = false);
	void whisperPlayer(Player *target, const string &whisperer_name, uint16_t channel, const string &message);
	void sendToPlayers(unsigned char *data, int32_t len);
	void useMeleeAttack(Player *player, const Attack &attack);
	void useRangedAttack(Player *player, const Attack &attack);
	void useSpellAttack(Player *player, const Attack &attack);
	void useSummonAttack(Player *player, const Attack &attack);
	void useEnergyChargeAttack(Player *player, const Attack &attack);
	void usePirateGrenade(Player *player, int32_t posx, int32_t posy, int32_t charge, int32_t unknown);
};
