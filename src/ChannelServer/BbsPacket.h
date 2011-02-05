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

#include "PacketCreator.h"
#include "PacketReader.h"
#include "Player.h"
#include "Types.h"
#include <string>

using std::string;

class Player;
class PacketCreator;
class PacketReader;

namespace BbsPacket {
	void handleBbsPacket(Player *player, PacketReader &packet);
	void newThread(Player *player, PacketReader &pack);
	void deleteThread(Player *player, PacketReader &pack);
	void sendThreadList(int16_t beginId, Player *player);
	void showThread(int32_t id, Player *player);
	void newReply(Player *player, PacketReader &pack);
	void deleteReply(Player *player, PacketReader &pack);
};
