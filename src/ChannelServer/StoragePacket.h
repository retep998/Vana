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

class Player;

namespace StoragePacket {
	void showStorage(Player *player, int32_t npcid);
	void addItem(Player *player, int8_t inv);
	void takeItem(Player *player, int8_t inv);
	void changeMesos(Player *player, int32_t mesos);
	void storageFull(Player *player);
	void noMesos(Player *player);
};
