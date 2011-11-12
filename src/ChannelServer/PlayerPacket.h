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

class KeyMaps;
class PacketCreator;
class Player;
class SkillMacros;

namespace PlayerPacket {
	namespace NoticeTypes {
		enum Types {
			Notice = 0x00,
			Box = 0x01,
			Red = 0x05,
			Blue = 0x06
		};
	}
	void connectData(Player *player);
	void showKeys(Player *player, KeyMaps *keymaps);
	void showSkillMacros(Player *player, SkillMacros *macros);
	void updateStatInt(Player *player, int32_t id, int32_t value, bool is = false);
	void updateStatShort(Player *player, int32_t id, int16_t value, bool is = false);
	void updateStatChar(Player *player, int32_t id, int8_t value, bool is = false);
	void changeChannel(Player *player, uint32_t ip, uint16_t port);
	void showMessage(Player *player, const string &msg, int8_t type);
	void showMessageChannel(const string &msg, int8_t type);
	void showMessageWorld(const string &msg, int8_t type);
	void showMessageGlobal(const string &msg, int8_t type);
	void showMessagePacket(PacketCreator &packet, const string &msg, int8_t type);
	void instructionBubble(Player *player, const string &msg, int16_t width = -1, int16_t time = 5, bool isStatic = false, int32_t x = 0, int32_t y = 0);
	void showHpBar(Player *player, Player *target);
	void sendBlockedMessage(Player *player, int8_t type);
	void sendYellowMessage(Player *player, const string &msg);
	void sendCannotFlyHere(Player *player);
};
