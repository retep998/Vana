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

class Item;
class CashItem;
class Player;
class PacketCreator;
struct ModifiedCashItem;

namespace PlayerPacketHelper {
	void addItemInfo(PacketCreator &packet, int16_t slot, Item *item, bool shortSlot = false);
	void addCashItemInfo(PacketCreator &packet, CashItem *item);
	void addPlayerDisplay(PacketCreator &packet, Player *player);
	void addModifiedCashInfo(PacketCreator &packet, ModifiedCashItem item);
}
