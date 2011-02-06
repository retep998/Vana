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
#ifndef PLAYERSPACKET_H
#define PLAYERSPACKET_H

#include "Types.h"
#include <string>

using std::string;

class PacketReader;
class Player;
struct ReturnDamageInfo;

namespace PlayersPacket {
	void showMoving(Player *player, unsigned char *buf, size_t size);
	void faceExpression(Player *player, int32_t face);
	void showChat(Player *player, const string &msg, bool bubbleOnly);
	void damagePlayer(Player *player, int32_t dmg, int32_t mob, uint8_t hit, uint8_t type, uint8_t stance, int32_t nodamageskill, const ReturnDamageInfo &pgmr);
	void showMessage(const string &msg, int8_t type);
	void showMessageWorld(const string &msg, int8_t type);
	void showInfo(Player *player, Player *getinfo, bool isself);
	void findPlayer(Player *player, const string &name, int32_t map, uint8_t is = 0, bool is_channel = 0);
	void whisperPlayer(Player *target, const string &whisperer_name, uint16_t channel, const string &message);
	void sendToPlayers(unsigned char *data, int32_t len);
	void useMeleeAttack(Player *player, PacketReader &pack);
	void useRangedAttack(Player *player, PacketReader &pack);
	void useSpellAttack(Player *player, PacketReader &pack);
	void useEnergyChargeAttack(Player *player, PacketReader &pack);
	void useSummonAttack(Player *player, PacketReader &pack);
};

#endif
