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
#include "Item.hpp"
#include "EquipDataProvider.hpp"
#include "GameLogicUtilities.hpp"
#include "ItemConstants.hpp"
#include "MiscUtilities.hpp"
#include "SociExtensions.hpp"
#include <soci.h>

const string_t Item::Inventory = "inventory";
const string_t Item::Storage = "storage";

// The only places that use this constructor set everything manually
Item::Item(const soci::row &row)
{
	initializeItem(row);
}

Item::Item(int32_t itemId, int16_t amount) :
	m_id(itemId),
	m_amount(amount)
{
}

Item::Item(int32_t equipId, bool random, bool isGm) :
	m_id(equipId),
	m_amount(1)
{
	EquipDataProvider::getInstance().setEquipStats(this, random, isGm);
}

Item::Item(Item *item)
{
	m_id = item->getId();
	m_amount = item->getAmount();
	m_hammers = item->getHammers();
	m_slots = item->getSlots();
	m_scrolls = item->getScrolls();
	m_str = item->getStr();
	m_dex = item->getDex();
	m_int = item->getInt();
	m_luk = item->getLuk();
	m_hp = item->getHp();
	m_mp = item->getMp();
	m_wAtk = item->getWatk();
	m_mAtk = item->getMatk();
	m_wDef = item->getWdef();
	m_mDef = item->getMdef();
	m_accuracy = item->getAccuracy();
	m_avoid = item->getAvoid();
	m_hands = item->getHands();
	m_jump = item->getJump();
	m_speed = item->getSpeed();
	m_petId = item->getPetId();
	m_name = item->getName();
	m_flags = item->getFlags();
	m_expiration = item->getExpirationTime();
}

auto Item::hasSlipPrevention() const -> bool {
	return testFlags(Items::Flags::Spikes);
}

auto Item::hasWarmSupport() const -> bool {
	return testFlags(Items::Flags::ColdProtection);
}

auto Item::hasLock() const -> bool {
	return testFlags(Items::Flags::Lock);
}

auto Item::hasKarma() const -> bool {
	return testFlags(Items::Flags::KarmaScissors);
}

auto Item::hasTradeBlock() const -> bool {
	return testFlags(Items::Flags::TradeUnavailable);
}

auto Item::testFlags(int16_t flags) const -> bool {
	return (m_flags & flags) != 0;
}

auto Item::setPreventSlip(bool prevent) -> void {
	modifyFlags(prevent, Items::Flags::Spikes);
}

auto Item::setWarmSupport(bool warm) -> void {
	modifyFlags(warm, Items::Flags::ColdProtection);
}

auto Item::setLock(bool lock) -> void {
	modifyFlags(lock, Items::Flags::Lock);
}

auto Item::setKarma(bool karma) -> void {
	modifyFlags(karma, Items::Flags::KarmaScissors);
}

auto Item::setTradeBlock(bool block) -> void {
	modifyFlags(block, Items::Flags::TradeUnavailable);
}

auto Item::modifyFlags(bool add, int16_t flags) -> void {
	if (add) {
		m_flags |= flags;
	}
	else {
		m_flags &= ~(flags);
	}
}

auto Item::setStr(int16_t strength) -> void {
	m_str = testStat(strength, Items::MaxStats::Str);
}

auto Item::setDex(int16_t dexterity) -> void {
	m_dex = testStat(dexterity, Items::MaxStats::Dex);
}

auto Item::setInt(int16_t intelligence) -> void {
	m_int = testStat(intelligence, Items::MaxStats::Int);
}

auto Item::setLuk(int16_t luck) -> void {
	m_luk = testStat(luck, Items::MaxStats::Luk);
}

auto Item::setHp(int16_t hp) -> void {
	m_hp = testStat(hp, Items::MaxStats::Hp);
}

auto Item::setMp(int16_t mp) -> void {
	m_mp = testStat(mp, Items::MaxStats::Mp);
}

auto Item::setWatk(int16_t wAtk) -> void {
	m_wAtk = testStat(wAtk, Items::MaxStats::Watk);
}

auto Item::setWdef(int16_t wDef) -> void {
	m_wDef = testStat(wDef, Items::MaxStats::Wdef);
}

auto Item::setMatk(int16_t mAtk) -> void {
	m_mAtk = testStat(mAtk, Items::MaxStats::Matk);
}

auto Item::setMdef(int16_t mDef) -> void {
	m_mDef = testStat(mDef, Items::MaxStats::Mdef);
}

auto Item::setAccuracy(int16_t acc) -> void {
	m_accuracy = testStat(acc, Items::MaxStats::Acc);
}

auto Item::setAvoid(int16_t avoid) -> void {
	m_avoid = testStat(avoid, Items::MaxStats::Avoid);
}

auto Item::setHands(int16_t hands) -> void {
	m_hands = testStat(hands, Items::MaxStats::Hands);
}

auto Item::setJump(int16_t jump) -> void {
	m_jump = testStat(jump, Items::MaxStats::Jump);
}

auto Item::setSpeed(int16_t speed) -> void {
	m_speed = testStat(speed, Items::MaxStats::Speed);
}

auto Item::testStat(int16_t stat, int16_t max) -> int16_t {
	return stat < 0 ? 0 : (stat > max ? max : stat);
}

auto Item::addStr(int16_t strength, bool onlyIfExists) -> void {
	if (testPerform(getStr(), onlyIfExists)) {
		setStr(getStr() + strength);
	}
}

auto Item::addDex(int16_t dexterity, bool onlyIfExists) -> void {
	if (testPerform(getDex(), onlyIfExists)) {
		setDex(getDex() + dexterity);
	}
}

auto Item::addInt(int16_t intelligence, bool onlyIfExists) -> void {
	if (testPerform(getInt(), onlyIfExists)) {
		setInt(getInt() + intelligence);
	}
}

auto Item::addLuk(int16_t luck, bool onlyIfExists) -> void {
	if (testPerform(getLuk(), onlyIfExists)) {
		setLuk(getLuk() + luck);
	}
}

auto Item::addHp(int16_t hp, bool onlyIfExists) -> void {
	if (testPerform(getHp(), onlyIfExists)) {
		setHp(getHp() + hp);
	}
}

auto Item::addMp(int16_t mp, bool onlyIfExists) -> void {
	if (testPerform(getMp(), onlyIfExists)) {
		setMp(getMp() + mp);
	}
}

auto Item::addWatk(int16_t wAtk, bool onlyIfExists) -> void {
	if (testPerform(getWatk(), onlyIfExists)) {
		setWatk(getWatk() + wAtk);
	}
}

auto Item::addWdef(int16_t wDef, bool onlyIfExists) -> void {
	if (testPerform(getWdef(), onlyIfExists)) {
		setWdef(getWdef() + wDef);
	}
}

auto Item::addMatk(int16_t mAtk, bool onlyIfExists) -> void {
	if (testPerform(getMatk(), onlyIfExists)) {
		setMatk(getMatk() + mAtk);
	}
}

auto Item::addMdef(int16_t mDef, bool onlyIfExists) -> void {
	if (testPerform(getMdef(), onlyIfExists)) {
		setMdef(getMdef() + mDef);
	}
}

auto Item::addAccuracy(int16_t acc, bool onlyIfExists) -> void {
	if (testPerform(getAccuracy(), onlyIfExists)) {
		setAccuracy(getAccuracy() + acc);
	}
}

auto Item::addAvoid(int16_t avoid, bool onlyIfExists) -> void {
	if (testPerform(getAvoid(), onlyIfExists)) {
		setAvoid(getAvoid() + avoid);
	}
}

auto Item::addHands(int16_t hands, bool onlyIfExists) -> void {
	if (testPerform(getHands(), onlyIfExists)) {
		setHands(getHands() + hands);
	}
}

auto Item::addJump(int16_t jump, bool onlyIfExists) -> void {
	if (testPerform(getJump(), onlyIfExists)) {
		setJump(getJump() + jump);
	}
}

auto Item::addSpeed(int16_t speed, bool onlyIfExists) -> void {
	if (testPerform(getSpeed(), onlyIfExists)) {
		setSpeed(getSpeed() + speed);
	}
}

auto Item::testPerform(int16_t stat, bool onlyIfExists) -> bool {
	return !onlyIfExists || stat != 0;
}

auto Item::setAmount(int16_t amount) -> void {
	m_amount = amount;
}

auto Item::setSlots(int8_t slots) -> void {
	m_slots = slots;
}

auto Item::setPetId(int64_t petId) -> void {
	m_petId = petId;
}

auto Item::setName(const string_t &name) -> void {
	m_name = name;
}

auto Item::initializeItem(const soci::row &row) -> void {
	m_id = row.get<int32_t>("item_id");
	m_amount = row.get<int16_t>("amount");

	opt_int8_t slots = row.get<opt_int8_t>("slots");
	opt_int8_t scrolls = row.get<opt_int8_t>("scrolls");
	opt_int16_t str = row.get<opt_int16_t>("istr");
	opt_int16_t dex = row.get<opt_int16_t>("idex");
	opt_int16_t intt = row.get<opt_int16_t>("iint");
	opt_int16_t luk = row.get<opt_int16_t>("iluk");
	opt_int16_t hp = row.get<opt_int16_t>("ihp");
	opt_int16_t mp = row.get<opt_int16_t>("imp");
	opt_int16_t watk = row.get<opt_int16_t>("iwatk");
	opt_int16_t matk = row.get<opt_int16_t>("imatk");
	opt_int16_t wdef = row.get<opt_int16_t>("iwdef");
	opt_int16_t mdef = row.get<opt_int16_t>("imdef");
	opt_int16_t accuracy = row.get<opt_int16_t>("iacc");
	opt_int16_t avoid = row.get<opt_int16_t>("iavo");
	opt_int16_t hands = row.get<opt_int16_t>("ihand");
	opt_int16_t speed = row.get<opt_int16_t>("ispeed");
	opt_int16_t jump = row.get<opt_int16_t>("ijump");
	opt_int16_t flags = row.get<opt_int16_t>("flags");
	opt_int32_t hammers = row.get<opt_int32_t>("hammers");
	opt_int64_t petId = row.get<opt_int64_t>("pet_id");
	opt_int64_t expiration = row.get<opt_int64_t>("expiration");
	opt_string_t name = row.get<opt_string_t>("name");

	m_slots = slots.getOrDefault(0);
	m_scrolls = scrolls.getOrDefault(0);
	m_str = str.getOrDefault(0);
	m_dex = dex.getOrDefault(0);
	m_int = intt.getOrDefault(0);
	m_luk = luk.getOrDefault(0);
	m_hp = hp.getOrDefault(0);
	m_mp = mp.getOrDefault(0);
	m_wAtk = watk.getOrDefault(0);
	m_mAtk = matk.getOrDefault(0);
	m_wDef = wdef.getOrDefault(0);
	m_mDef = mdef.getOrDefault(0);
	m_accuracy = accuracy.getOrDefault(0);
	m_avoid = avoid.getOrDefault(0);
	m_hands = hands.getOrDefault(0);
	m_speed = speed.getOrDefault(0);
	m_jump = jump.getOrDefault(0);
	m_hammers = hammers.getOrDefault(0);
	m_flags = flags.getOrDefault(0);
	m_expiration = expiration.getOrDefault(Items::NoExpiration);
	m_petId = petId.getOrDefault(0);
	m_name = name.getOrDefault("");
}

auto Item::databaseInsert(soci::session &sql, const ItemDbInformation &info) -> void {
	vector_t<ItemDbRecord> v;
	ItemDbRecord r(info, this);
	v.push_back(r);
	Item::databaseInsert(sql, v);
}

auto Item::databaseInsert(soci::session &sql, const vector_t<ItemDbRecord> &items) -> void {
	using namespace soci;
	using MiscUtilities::getOptional;

	static init_list_t<int8_t> nullsInt8 = {0};
	static init_list_t<int16_t> nullsInt16 = {0};
	static init_list_t<int32_t> nullsInt32 = {0};
	static init_list_t<int64_t> nullsInt64 = {0};
	static init_list_t<int64_t> nullsExpiration = {0, Items::NoExpiration};
	static init_list_t<string_t> nullsString = {""};

	uint8_t inventory = 0;
	int16_t amount = 0;
	int32_t itemId = 0;
	int16_t slot = 0;
	world_id_t worldId = 0;
	int32_t userId = 0;
	int32_t playerId = 0;
	string_t location = "";

	opt_int8_t slots;
	opt_int8_t scrolls;
	opt_int16_t iStr;
	opt_int16_t iDex;
	opt_int16_t iInt;
	opt_int16_t iLuk;
	opt_int16_t iHp;
	opt_int16_t iMp;
	opt_int16_t iWatk;
	opt_int16_t iMatk;
	opt_int16_t iWdef;
	opt_int16_t iMdef;
	opt_int16_t iAcc;
	opt_int16_t iAvo;
	opt_int16_t iHands;
	opt_int16_t iSpeed;
	opt_int16_t iJump;
	opt_int16_t flags;
	opt_int32_t hammers;
	opt_int64_t petId;
	opt_int64_t expiration;
	opt_string_t name;

	statement st = (sql.prepare
		<< "INSERT INTO " << Database::makeCharTable("items") << " (character_id, inv, slot, location, user_id, world_id, item_id, amount, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, flags, hammers, pet_id, name, expiration) "
		<< "VALUES (:char, :inv, :slot, :location, :user, :world, :itemid, :amount, :slots, :scrolls, :str, :dex, :int, :luk, :hp, :mp, :watk, :matk, :wdef, :mdef, :acc, :avo, :hands, :speed, :jump, :flags, :hammers, :pet, :name, :expiration)",
		use(playerId, "char"),
		use(inventory, "inv"),
		use(slot, "slot"),
		use(location, "location"),
		use(userId, "user"),
		use(worldId, "world"),
		use(itemId, "itemid"),
		use(amount, "amount"),
		use(slots, "slots"),
		use(scrolls, "scrolls"),
		use(iStr, "str"),
		use(iDex, "dex"),
		use(iInt, "int"),
		use(iLuk, "luk"),
		use(iHp, "hp"),
		use(iMp, "mp"),
		use(iWatk, "watk"),
		use(iMatk, "matk"),
		use(iWdef, "wdef"),
		use(iMdef, "mdef"),
		use(iAcc, "acc"),
		use(iAvo, "avo"),
		use(iHands, "hands"),
		use(iSpeed, "speed"),
		use(iJump, "jump"),
		use(flags, "flags"),
		use(hammers, "hammers"),
		use(petId, "pet"),
		use(name, "name"),
		use(expiration, "expiration"));

	for (const auto &rec : items) {
		Item *item = rec.item;

		location = rec.location;
		userId = rec.userId;
		playerId = rec.charId;
		worldId = rec.worldId;
		slot = rec.slot;
		amount = item->m_amount;
		itemId = item->m_id;
		inventory = GameLogicUtilities::getInventory(itemId);
		bool equip = (inventory == Inventories::EquipInventory);
		MiscUtilities::NullableMode nulls = (equip ? MiscUtilities::NullableMode::NullIfFound : MiscUtilities::NullableMode::ForceNull);
		MiscUtilities::NullableMode required = (equip ? MiscUtilities::NullableMode::ForceNotNull : MiscUtilities::NullableMode::ForceNull);

		slots = getOptional(item->m_slots, required, nullsInt8);
		scrolls = getOptional(item->m_scrolls, required, nullsInt8);
		iStr = getOptional(item->m_str, nulls, nullsInt16);
		iDex = getOptional(item->m_dex, nulls, nullsInt16);
		iInt = getOptional(item->m_int, nulls, nullsInt16);
		iLuk = getOptional(item->m_luk, nulls, nullsInt16);
		iHp = getOptional(item->m_hp, nulls, nullsInt16);
		iMp = getOptional(item->m_mp, nulls, nullsInt16);
		iWatk = getOptional(item->m_wAtk, nulls, nullsInt16);
		iMatk = getOptional(item->m_mAtk, nulls, nullsInt16);
		iWdef = getOptional(item->m_wDef, nulls, nullsInt16);
		iMdef = getOptional(item->m_mDef, nulls, nullsInt16);
		iAcc = getOptional(item->m_accuracy, nulls, nullsInt16);
		iAvo = getOptional(item->m_avoid, nulls, nullsInt16);
		iHands = getOptional(item->m_hands, nulls, nullsInt16);
		iSpeed = getOptional(item->m_speed, nulls, nullsInt16);
		iJump = getOptional(item->m_jump, nulls, nullsInt16);
		flags = getOptional(item->m_flags, nulls, nullsInt16);
		hammers = getOptional(item->m_hammers, nulls, nullsInt32);
		petId = getOptional(item->m_petId, nulls, nullsInt64);
		name = getOptional(item->m_name, nulls, nullsString);
		expiration = getOptional(item->m_expiration, nulls, nullsExpiration);

		st.execute(true);
	}
}