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

#include "Types.hpp"
#include <string>
#include <vector>

namespace soci {
	class row;
	class session;
}
class Item;

struct ItemDbInformation {
	NO_DEFAULT_CONSTRUCTOR(ItemDbInformation);
public:
	ItemDbInformation(int16_t slot, int32_t charId, int32_t userId, world_id_t worldId, const string_t &location) :
		slot(slot),
		charId(charId),
		userId(userId),
		worldId(worldId),
		location(location)
	{
	}

	int16_t slot;
	int32_t charId;
	int32_t userId;
	world_id_t worldId;
	string_t location;
};

struct ItemDbRecord : ItemDbInformation {
	NO_DEFAULT_CONSTRUCTOR(ItemDbRecord);
public:
	ItemDbRecord(int16_t slot, int32_t charId, int32_t userId, world_id_t worldId, const string_t &location, Item *item) :
		ItemDbInformation(slot, charId, userId, worldId, location),
		item(item)
	{
	}
	ItemDbRecord(const ItemDbInformation &info, Item *item) :
		ItemDbInformation(info.slot, info.charId, info.userId, info.worldId, info.location),
		item(item)
	{
	}

	Item *item;
};

class Item {
public:
	Item() = default;
	Item(const soci::row &row);
	Item(int32_t itemId, int16_t amount);
	Item(int32_t equipId, bool random, bool isGm);
	Item(Item *item);

	auto hasWarmSupport() const -> bool;
	auto hasSlipPrevention() const -> bool;
	auto hasLock() const -> bool;
	auto hasKarma() const -> bool;
	auto hasTradeBlock() const -> bool;

	auto getSlots() const -> int8_t { return m_slots; }
	auto getScrolls() const -> int8_t { return m_scrolls; }
	auto getStr() const -> int16_t { return m_str; }
	auto getDex() const -> int16_t { return m_dex; }
	auto getInt() const -> int16_t { return m_int; }
	auto getLuk() const -> int16_t { return m_luk; }
	auto getHp() const -> int16_t { return m_hp; }
	auto getMp() const -> int16_t { return m_mp; }
	auto getWatk() const -> int16_t { return m_wAtk; }
	auto getMatk() const -> int16_t { return m_mAtk; }
	auto getWdef() const -> int16_t { return m_wDef; }
	auto getMdef() const -> int16_t { return m_mDef; }
	auto getAccuracy() const -> int16_t { return m_accuracy; }
	auto getAvoid() const -> int16_t { return m_avoid; }
	auto getHands() const -> int16_t { return m_hands; }
	auto getSpeed() const -> int16_t { return m_speed; }
	auto getJump() const -> int16_t { return m_jump; }
	auto getAmount() const -> int16_t { return m_amount; }
	auto getFlags() const -> int16_t { return m_flags; }
	auto getId() const -> int32_t { return m_id; }
	auto getHammers() const -> int32_t { return m_hammers; }
	auto getPetId() const -> int64_t { return m_petId; }
	auto getExpirationTime() const -> int64_t { return m_expiration; }
	auto getName() const -> const string_t & { return m_name; }

	auto setPreventSlip(bool prevent) -> void;
	auto setWarmSupport(bool warm) -> void;
	auto setLock(bool lock) -> void;
	auto setKarma(bool karma) -> void;
	auto setTradeBlock(bool block) -> void;

	auto databaseInsert(soci::session &sql, const ItemDbInformation &info) -> void;
	auto setSlots(int8_t slots) -> void;
	auto setScrolls(int8_t scrolls) -> void;
	auto setStr(int16_t strength) -> void;
	auto setDex(int16_t dexterity) -> void;
	auto setInt(int16_t intelligence) -> void;
	auto setLuk(int16_t luck) -> void;
	auto setHp(int16_t hp) -> void;
	auto setMp(int16_t mp) -> void;
	auto setWatk(int16_t wAtk) -> void;
	auto setWdef(int16_t wDef) -> void;
	auto setMatk(int16_t mAtk) -> void;
	auto setMdef(int16_t mDef) -> void;
	auto setAccuracy(int16_t acc) -> void;
	auto setAvoid(int16_t avoid) -> void;
	auto setHands(int16_t hands) -> void;
	auto setJump(int16_t jump) -> void;
	auto setSpeed(int16_t speed) -> void;
	auto setAmount(int16_t amount) -> void;
	auto setName(const string_t &name) -> void;
	auto setFlags(int16_t flags) -> void;
	auto setHammers(int32_t hammers) -> void;
	auto setPetId(int64_t petId) -> void;
	auto setExpirationTime(int64_t exp) -> void;
	auto addStr(int16_t strength, bool onlyIfExists = false) -> void;
	auto addDex(int16_t dexterity, bool onlyIfExists = false) -> void;
	auto addInt(int16_t intelligence, bool onlyIfExists = false) -> void;
	auto addLuk(int16_t luck, bool onlyIfExists = false) -> void;
	auto addHp(int16_t hp, bool onlyIfExists = false) -> void;
	auto addMp(int16_t mp, bool onlyIfExists = false) -> void;
	auto addWatk(int16_t wAtk, bool onlyIfExists = false) -> void;
	auto addWdef(int16_t wDef, bool onlyIfExists = false) -> void;
	auto addMatk(int16_t mAtk, bool onlyIfExists = false) -> void;
	auto addMdef(int16_t mDef, bool onlyIfExists = false) -> void;
	auto addAccuracy(int16_t acc, bool onlyIfExists = false) -> void;
	auto addAvoid(int16_t avoid, bool onlyIfExists = false) -> void;
	auto addHands(int16_t hands, bool onlyIfExists = false) -> void;
	auto addJump(int16_t jump, bool onlyIfExists = false) -> void;
	auto addSpeed(int16_t speed, bool onlyIfExists = false) -> void;
	auto incAmount(int16_t mod) -> void { m_amount += mod; }
	auto decAmount(int16_t mod) -> void { m_amount -= mod; }
	auto incHammers() -> void { m_hammers++; }
	auto incSlots(int8_t inc = 1) -> void { m_slots += inc; }
	auto decSlots(int8_t dec = 1) -> void { m_slots -= dec; }
	auto incScrolls() -> void { m_scrolls++; }

	static auto databaseInsert(soci::session &sql, const vector_t<ItemDbRecord> &items) -> void;

	const static string_t Inventory;
	const static string_t Storage;
private:
	auto testPerform(int16_t stat, bool onlyIfExists) -> bool;
	auto testStat(int16_t stat, int16_t max) -> int16_t;
	auto modifyFlags(bool add, int16_t flags) -> void;
	auto testFlags(int16_t flags) const -> bool;
	auto initializeItem(const soci::row &row) -> void;

	int8_t m_slots = 0;
	int8_t m_scrolls = 0;
	int16_t m_str = 0;
	int16_t m_dex = 0;
	int16_t m_int = 0;
	int16_t m_luk = 0;
	int16_t m_hp = 0;
	int16_t m_mp = 0;
	int16_t m_wAtk = 0;
	int16_t m_mAtk = 0;
	int16_t m_wDef = 0;
	int16_t m_mDef = 0;
	int16_t m_accuracy = 0;
	int16_t m_avoid = 0;
	int16_t m_hands = 0;
	int16_t m_jump = 0;
	int16_t m_speed = 0;
	int16_t m_flags = 0;
	int16_t m_amount = 0;
	int32_t m_id = 0;
	int32_t m_hammers = 0;
	int64_t m_petId = 0;
	int64_t m_expiration = Items::NoExpiration;
	string_t m_name;
};