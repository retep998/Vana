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

class Player;
class PacketReader;

namespace PetHandler {
	void showPets(Player *player);
	void handleChat(Player *player, PacketReader &packet);
	void handleFeed(Player *player, PacketReader &packet);
	void handleMovement(Player *player, PacketReader &packet);
	void handleSummon(Player *player, PacketReader &packet);
	void handleCommand(Player *player, PacketReader &packet);
	void handleConsumePotion(Player *player, PacketReader &packet);
	void changeName(Player *player, const string &name);
};