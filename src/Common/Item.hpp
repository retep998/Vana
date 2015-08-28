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

#include "FileTime.hpp"
#include "ItemConstants.hpp"
#include "Types.hpp"
#include <string>
#include <vector>

namespace Vana {
	class Database;
	class EquipDataProvider;
	class Item;
	class soci::row;

	struct ItemDbInformation {
		NO_DEFAULT_CONSTRUCTOR(ItemDbInformation);
	public:
		ItemDbInformation(inventory_slot_t slot, player_id_t charId, account_id_t userId, world_id_t worldId, const string_t &location) :
			slot{slot},
			charId{charId},
			userId{userId},
			worldId{worldId},
			location{location}
		{
		}

		inventory_slot_t slot;
		player_id_t charId;
		account_id_t userId;
		world_id_t worldId;
		string_t location;
	};

	struct ItemDbRecord : ItemDbInformation {
		NO_DEFAULT_CONSTRUCTOR(ItemDbRecord);
	public:
		ItemDbRecord(inventory_slot_t slot, player_id_t charId, account_id_t userId, world_id_t worldId, const string_t &location, Item *item) :
			ItemDbInformation{slot, charId, userId, worldId, location},
			item{item}
		{
		}
		ItemDbRecord(const ItemDbInformation &info, Item *item) :
			ItemDbInformation{info.slot, info.charId, info.userId, info.worldId, info.location},
			item{item}
		{
		}

		Item *item;
	};

	class Item {
	public:
		Item() = default;
		Item(const soci::row &row);
		Item(item_id_t itemId, slot_qty_t amount);
		Item(const EquipDataProvider &provider, item_id_t equipId, Items::StatVariance variancePolicy, bool isGm);
		Item(Item *item);

		auto hasWarmSupport() const -> bool;
		auto hasSlipPrevention() const -> bool;
		auto hasLock() const -> bool;
		auto hasKarma() const -> bool;
		auto hasTradeBlock() const -> bool;

		auto getSlots() const -> int8_t { return m_slots; }
		auto getScrolls() const -> int8_t { return m_scrolls; }
		auto getStr() const -> stat_t { return m_str; }
		auto getDex() const -> stat_t { return m_dex; }
		auto getInt() const -> stat_t { return m_int; }
		auto getLuk() const -> stat_t { return m_luk; }
		auto getHp() const -> health_t { return m_hp; }
		auto getMp() const -> health_t { return m_mp; }
		auto getWatk() const -> stat_t { return m_wAtk; }
		auto getMatk() const -> stat_t { return m_mAtk; }
		auto getWdef() const -> stat_t { return m_wDef; }
		auto getMdef() const -> stat_t { return m_mDef; }
		auto getAccuracy() const -> stat_t { return m_accuracy; }
		auto getAvoid() const -> stat_t { return m_avoid; }
		auto getHands() const -> stat_t { return m_hands; }
		auto getSpeed() const -> stat_t { return m_speed; }
		auto getJump() const -> stat_t { return m_jump; }
		auto getAmount() const -> slot_qty_t { return m_amount; }
		auto getFlags() const -> int16_t { return m_flags; }
		auto getId() const -> item_id_t { return m_id; }
		auto getHammers() const -> int32_t { return m_hammers; }
		auto getPetId() const -> pet_id_t { return m_petId; }
		auto getExpirationTime() const -> FileTime { return m_expiration; }
		auto getName() const -> const string_t & { return m_name; }

		auto setPreventSlip(bool prevent) -> void;
		auto setWarmSupport(bool warm) -> void;
		auto setLock(bool lock) -> void;
		auto setKarma(bool karma) -> void;
		auto setTradeBlock(bool block) -> void;

		auto databaseInsert(Database &db, const ItemDbInformation &info) -> void;
		auto setSlots(int8_t slots) -> void;
		auto setStr(stat_t strength) -> void;
		auto setDex(stat_t dexterity) -> void;
		auto setInt(stat_t intelligence) -> void;
		auto setLuk(stat_t luck) -> void;
		auto setHp(health_t hp) -> void;
		auto setMp(health_t mp) -> void;
		auto setWatk(stat_t wAtk) -> void;
		auto setWdef(stat_t wDef) -> void;
		auto setMatk(stat_t mAtk) -> void;
		auto setMdef(stat_t mDef) -> void;
		auto setAccuracy(stat_t acc) -> void;
		auto setAvoid(stat_t avoid) -> void;
		auto setHands(stat_t hands) -> void;
		auto setJump(stat_t jump) -> void;
		auto setSpeed(stat_t speed) -> void;
		auto setAmount(slot_qty_t amount) -> void;
		auto setName(const string_t &name) -> void;
		auto setPetId(pet_id_t petId) -> void;
		auto addStr(stat_t strength) -> void;
		auto addDex(stat_t dexterity) -> void;
		auto addInt(stat_t intelligence) -> void;
		auto addLuk(stat_t luck) -> void;
		auto addHp(health_t hp) -> void;
		auto addMp(health_t mp) -> void;
		auto addWatk(stat_t wAtk) -> void;
		auto addWdef(stat_t wDef) -> void;
		auto addMatk(stat_t mAtk) -> void;
		auto addMdef(stat_t mDef) -> void;
		auto addAccuracy(stat_t acc) -> void;
		auto addAvoid(stat_t avoid) -> void;
		auto addHands(stat_t hands) -> void;
		auto addJump(stat_t jump) -> void;
		auto addSpeed(stat_t speed) -> void;
		auto incAmount(slot_qty_t mod) -> void { m_amount += mod; }
		auto decAmount(slot_qty_t mod) -> void { m_amount -= mod; }
		auto incHammers() -> void { m_hammers++; }
		auto incSlots(int8_t inc = 1) -> void { m_slots += inc; }
		auto decSlots(int8_t dec = 1) -> void { m_slots -= dec; }
		auto incScrolls() -> void { m_scrolls++; }

		static auto databaseInsert(Database &db, const vector_t<ItemDbRecord> &items) -> void;

		const static string_t Inventory;
		const static string_t Storage;
	private:
		auto testStat(int16_t stat, int16_t max) -> int16_t;
		auto modifyFlags(bool add, int16_t flags) -> void;
		auto testFlags(int16_t flags) const -> bool;
		auto initializeItem(const soci::row &row) -> void;

		int8_t m_slots = 0;
		int8_t m_scrolls = 0;
		stat_t m_str = 0;
		stat_t m_dex = 0;
		stat_t m_int = 0;
		stat_t m_luk = 0;
		health_t m_hp = 0;
		health_t m_mp = 0;
		stat_t m_wAtk = 0;
		stat_t m_mAtk = 0;
		stat_t m_wDef = 0;
		stat_t m_mDef = 0;
		stat_t m_accuracy = 0;
		stat_t m_avoid = 0;
		stat_t m_hands = 0;
		stat_t m_jump = 0;
		stat_t m_speed = 0;
		int16_t m_flags = 0;
		slot_qty_t m_amount = 0;
		item_id_t m_id = 0;
		int32_t m_hammers = 0;
		pet_id_t m_petId = 0;
		FileTime m_expiration = Items::NoExpiration;
		string_t m_name;
	};
}