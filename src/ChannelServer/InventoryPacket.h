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
#include <vector>
#include <string>

using std::string;
using std::vector;

class Item;
class Player;

namespace InventoryPacket {
	namespace RockModes {
		enum Modes {
			Delete = 0x02,
			Add = 0x03
		};
	}
	namespace RockErrors {
		enum Errors {
			Unk = 0x02, // Causes error 38 with current data
			Unk2 = 0x03, // Causes error 38 with current data
			CannotGo2 = 0x05, // This is unused
			DifficultToLocate = 0x06,
			DifficultToLocate2 = 0x07, // This is unused
			CannotGo = 0x08,
			AlreadyThere = 0x09,
			CannotSaveMap = 0x0A,
			NoobsCannotLeaveMapleIsland = 0x0B // "Users below level 7 are not allowed to go out from Maple Island."
		};
	}
	namespace RockTypes {
		enum Types {
			Regular = 0x00,
			Vip = 0x01
		};
	}

	void moveItem(Player *player, int8_t inv, int16_t slot1, int16_t slot2);
	void updatePlayer(Player *player);
	void addItem(Player *player, int8_t inv, int16_t slot, Item *item, bool is);
	void addNewItem(Player *player, int8_t inv, int16_t slot, Item *item, bool is);
	void updateItemAmounts(Player *player, int8_t inv, int16_t slot1, int16_t amount1, int16_t slot2, int16_t amount2);
	void sitChair(Player *player, int32_t chairid);
	void sitMapChair(Player *player, int16_t chairid);
	void stopChair(Player *player, bool showMap = true);
	void useSkillbook(Player *player, int32_t skillid, int32_t newMaxLevel, bool use, bool succeed);
	void useScroll(Player *player, int8_t succeed, bool destroy, bool legendarySpirit);
	void showMegaphone(Player *player, const string &msg);
	void showSuperMegaphone(Player *player, const string &msg, bool whisper = false);
	void showMessenger(Player *player, const string &msg, const string &msg2, const string &msg3, const string &msg4, unsigned char *displayInfo, int32_t displayInfo_size, int32_t itemid);
	void showItemMegaphone(Player *player, const string &msg, bool whisper = false, Item *item = nullptr);
	void showTripleMegaphone(Player *player, int8_t lines, const string &line1, const string &line2, const string &line3, bool whisper);
	void useItemEffect(Player *player, int32_t itemid);
	void updateSlots(Player *player, int8_t inventory, int8_t slots);
	void sendRockUpdate(Player *player, int8_t mode, int8_t type, const vector<int32_t> &maps);
	void sendRockError(Player *player, int8_t code, int8_t type);
	void useCharm(Player *player, uint8_t charmsleft, uint8_t daysleft = 99);
	void sendMesobagSucceed(Player *player, int32_t mesos);
	void sendMesobagFailed(Player *player);
	void sendHammerSlots(Player *player, int32_t slots);
	void sendHulkSmash(Player *player, int16_t slot, Item *hammered);
	void sendHammerUpdate(Player *player);
	void sendChalkboardUpdate(Player *player, const string &msg = "");
	void playCashSong(int32_t map, int32_t itemid, const string &playername);
	void playCashSongPlayer(Player *player, int32_t itemid, const string &playername);
	void sendRewardItemAnimation(Player *player, int32_t itemid, const string &effect);
	void blankUpdate(Player *player);
	void sendItemExpired(Player *player, vector<int32_t> *items);
	void sendCannotFlyHere(Player *player);
};
