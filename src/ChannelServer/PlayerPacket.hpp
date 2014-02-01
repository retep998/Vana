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

#include "Ip.hpp"
#include "PacketBuilder.hpp"
#include "Types.hpp"
#include <string>

class KeyMaps;
class PacketBuilder;
class Player;
class SkillMacros;

namespace PlayerPacket {
	namespace BlockMessages {
		enum Messages : int8_t {
			CannotGo = 0x01,
			NoCashShop = 0x02,
			MtsUnavailable = 0x03,
			MtsUserLimit = 0x04,
			LevelTooLow = 0x05
		};
	}
	namespace NoticeTypes {
		enum Types : int8_t {
			Notice = 0x00,
			Box = 0x01,
			Red = 0x05,
			Blue = 0x06
		};
	}

	PACKET(connectData, Player *player);
	PACKET(showKeys, KeyMaps *keymaps);
	PACKET(showSkillMacros, SkillMacros *macros);
	PACKET(updateStat, int32_t updateBits, int32_t value, bool itemResponse = false);
	PACKET(changeChannel, const Ip &ip, port_t port);
	PACKET(showMessage, const string_t &msg, int8_t type);
	PACKET(groupChat, const string_t &name, const string_t &msg, int8_t type);
	PACKET(instructionBubble, const string_t &msg, int16_t width = -1, int16_t time = 5, bool isStatic = false, int32_t x = 0, int32_t y = 0);
	PACKET(showHpBar, int32_t playerId, int32_t hp, int32_t maxHp);
	PACKET(sendBlockedMessage, int8_t type);
	PACKET(sendYellowMessage, const string_t &msg);
}