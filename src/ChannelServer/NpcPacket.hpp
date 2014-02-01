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
#include "Types.hpp"

class Player;
class PacketReader;
struct NpcSpawnInfo;
struct BuiltShopInfo;

namespace NpcPacket {
	namespace BoughtMessages {
		enum {
			Success = 0,
			NotEnoughInStock = 1,
			NotEnoughMesos = 2,
			NoSlots = 3
		};
	}
	namespace Dialogs {
		enum DialogOptions : int8_t {
			Normal = 0x00,
			YesNo = 0x01,
			GetText = 0x02,
			GetNumber = 0x03,
			Simple = 0x04,
			Question = 0x05,
			Quiz = 0x06,
			Style = 0x07,
			AcceptDecline = 0x0C,
			AcceptDeclineNoExit = 0x0D
		};
	}

	PACKET(showNpc, const NpcSpawnInfo &npc, int32_t id, bool show = true);
	PACKET(controlNpc, const NpcSpawnInfo &npc, int32_t id, bool show = true);
	PACKET(animateNpc, PacketReader &reader);
	PACKET(showNpcEffect, int32_t index, bool show = false);
	PACKET(bought, uint8_t msg);
	PACKET(showShop, const BuiltShopInfo &shop, int16_t rechargeableBonus);
	PACKET(npcChat, int8_t type, int32_t npcId, const string_t &text, bool excludeText = false);
}