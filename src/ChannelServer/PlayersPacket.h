/*
Copyright (C) 2008 Vana Development Team

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
#include <vector>

using std::string;
using std::vector;

class Player;
class PacketCreator;

struct PGMRInfo;

namespace PlayersPacket {
	void showMoving(Player *player, unsigned char *buf, size_t size);
	void faceExpression(Player *player, int32_t face);
	void showChat(Player *player, const string &msg, char bubbleOnly);
	void damagePlayer(Player *player, int32_t dmg, int32_t mob, unsigned char hit, unsigned char type, unsigned char stance, int32_t nodamageskill, PGMRInfo pgmr);
	void showMessage(const string &msg, char type);
	void showInfo(Player *player, Player *getinfo, unsigned char isself);
	void findPlayer(Player *player, const string &name, int32_t map, unsigned char is = 0, bool is_channel = 0);
	void whisperPlayer(Player *target, const string &whisperer_name, uint16_t channel, const string &message);
	void sendToPlayers(unsigned char *data, int32_t len);
};

#endif
