/*
Copyright (C) 2008-2015 Vana Development Team

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

#include "Common/Types.hpp"

namespace Vana {
	enum class KeyMapAction : int32_t {
		EquipmentMenu = 0,
		ItemMenu = 1,
		AbilityMenu = 2,
		SkillMenu = 3,
		BuddyList = 4,
		WorldMap = 5,
		Messenger = 6,
		Minimap = 7,
		QuestMenu = 8,
		SetKey = 9,
		AllChat = 10,
		WhisperChat = 11,
		PartyChat = 12,
		BuddyChat = 13,
		Shortcut = 14,
		QuickSlot = 15,
		ExpandChat = 16,
		GuildList = 17,
		GuildChat = 18,
		PartyList = 19,
		Helper = 20,
		SpouseChat = 21,
		MonsterBook = 22,
		CashShop = 23,
		AllianceChat = 24,
		PartySearch = 25,
		FamilyList = 26,

		PickUp = 50,
		Sit = 51,
		Attack = 52,
		Jump = 53,
		NpcChat = 54,

		Cockeyed = 100,
		Happy = 101,
		Sarcastic = 102,
		Crying = 103,
		Outraged = 104,
		Shocked = 105,
		Annoyed = 106,
	};
}