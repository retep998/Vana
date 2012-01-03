/*
Copyright (C) 2008-2012 Vana Development Team

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

class KeyMaps;
class PacketCreator;
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
	void connectData(Player *player);
	void showKeys(Player *player, KeyMaps *keymaps);
	void showSkillMacros(Player *player, SkillMacros *macros);
	void updateStat(Player *player, int32_t updateBits, int32_t value, bool itemResponse = false);
	void changeChannel(Player *player, ip_t ip, port_t port);
	void showMessage(Player *player, const string &msg, int8_t type);
	void showMessageChannel(const string &msg, int8_t type);
	void showMessageWorld(const string &msg, int8_t type);
	void showMessageGlobal(const string &msg, int8_t type);
	void showMessagePacket(PacketCreator &packet, const string &msg, int8_t type);
	void instructionBubble(Player *player, const string &msg, int16_t width = -1, int16_t time = 5, bool isStatic = false, int32_t x = 0, int32_t y = 0);
	void showHpBar(Player *player, Player *target);
	void sendBlockedMessage(Player *player, int8_t type);
	void sendYellowMessage(Player *player, const string &msg);
}