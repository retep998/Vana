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
#include "Item.hpp"
#include "Common/EquipDataProvider.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/ItemConstants.hpp"
#include "Common/MiscUtilities.hpp"
#include "Common/SociExtensions.hpp"
#include <soci.h>

namespace Vana {

const string_t Item::Inventory = "inventory";
const string_t Item::Storage = "storage";

// The only places that use this constructor set everything manually
Item::Item(const soci::row &row)
{
	initializeItem(row);
}

Item::Item(item_id_t itemId, slot_qty_t amount) :
	m_id{itemId},
	m_amount{amount}
{
}

Item::Item(const EquipDataProvider &provider, item_id_t equipId, Items::StatVariance variancePolicy, bool isGm) :
	m_id{equipId},
	m_amount{1}
{
	provider.setEquipStats(this, variancePolicy, isGm, true);
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

auto Item::setStr(stat_t strength) -> void {
	m_str = testStat(strength, Items::MaxStats::Str);
}

auto Item::setDex(stat_t dexterity) -> void {
	m_dex = testStat(dexterity, Items::MaxStats::Dex);
}

auto Item::setInt(stat_t intelligence) -> void {
	m_int = testStat(intelligence, Items::MaxStats::Int);
}

auto Item::setLuk(stat_t luck) -> void {
	m_luk = testStat(luck, Items::MaxStats::Luk);
}

auto Item::setHp(health_t hp) -> void {
	m_hp = testStat(hp, Items::MaxStats::Hp);
}

auto Item::setMp(health_t mp) -> void {
	m_mp = testStat(mp, Items::MaxStats::Mp);
}

auto Item::setWatk(stat_t wAtk) -> void {
	m_wAtk = testStat(wAtk, Items::MaxStats::Watk);
}

auto Item::setWdef(stat_t wDef) -> void {
	m_wDef = testStat(wDef, Items::MaxStats::Wdef);
}

auto Item::setMatk(stat_t mAtk) -> void {
	m_mAtk = testStat(mAtk, Items::MaxStats::Matk);
}

auto Item::setMdef(stat_t mDef) -> void {
	m_mDef = testStat(mDef, Items::MaxStats::Mdef);
}

auto Item::setAccuracy(stat_t acc) -> void {
	m_accuracy = testStat(acc, Items::MaxStats::Acc);
}

auto Item::setAvoid(stat_t avoid) -> void {
	m_avoid = testStat(avoid, Items::MaxStats::Avoid);
}

auto Item::setHands(stat_t hands) -> void {
	m_hands = testStat(hands, Items::MaxStats::Hands);
}

auto Item::setJump(stat_t jump) -> void {
	m_jump = testStat(jump, Items::MaxStats::Jump);
}

auto Item::setSpeed(stat_t speed) -> void {
	m_speed = testStat(speed, Items::MaxStats::Speed);
}

auto Item::testStat(int16_t stat, int16_t max) -> int16_t {
	return stat < 0 ? 0 : (stat > max ? max : stat);
}

auto Item::addStr(stat_t strength) -> void {
	setStr(getStr() + strength);
}

auto Item::addDex(stat_t dexterity) -> void {
	setDex(getDex() + dexterity);
}

auto Item::addInt(stat_t intelligence) -> void {
	setInt(getInt() + intelligence);
}

auto Item::addLuk(stat_t luck) -> void {
	setLuk(getLuk() + luck);
}

auto Item::addHp(health_t hp) -> void {
	setHp(getHp() + hp);
}

auto Item::addMp(health_t mp) -> void {
	setMp(getMp() + mp);
}

auto Item::addWatk(stat_t wAtk) -> void {
	setWatk(getWatk() + wAtk);
}

auto Item::addWdef(stat_t wDef) -> void {
	setWdef(getWdef() + wDef);
}

auto Item::addMatk(stat_t mAtk) -> void {
	setMatk(getMatk() + mAtk);
}

auto Item::addMdef(stat_t mDef) -> void {
	setMdef(getMdef() + mDef);
}

auto Item::addAccuracy(stat_t acc) -> void {
	setAccuracy(getAccuracy() + acc);
}

auto Item::addAvoid(stat_t avoid) -> void {
	setAvoid(getAvoid() + avoid);
}

auto Item::addHands(stat_t hands) -> void {
	setHands(getHands() + hands);
}

auto Item::addJump(stat_t jump) -> void {
	setJump(getJump() + jump);
}

auto Item::addSpeed(stat_t speed) -> void {
	setSpeed(getSpeed() + speed);
}

auto Item::setAmount(slot_qty_t amount) -> void {
	m_amount = amount;
}

auto Item::setSlots(int8_t slots) -> void {
	m_slots = slots;
}

auto Item::setPetId(pet_id_t petId) -> void {
	m_petId = petId;
}

auto Item::setName(const string_t &name) -> void {
	m_name = name;
}

auto Item::initializeItem(const soci::row &row) -> void {
	m_id = row.get<item_id_t>("item_id");
	m_amount = row.get<slot_qty_t>("amount");

	using opt_stat_t = optional_t<stat_t>;
	using opt_health_t = optional_t<health_t>;

	opt_int8_t slots = row.get<opt_int8_t>("slots");
	opt_int8_t scrolls = row.get<opt_int8_t>("scrolls");
	opt_stat_t str = row.get<opt_stat_t>("istr");
	opt_stat_t dex = row.get<opt_stat_t>("idex");
	opt_stat_t intt = row.get<opt_stat_t>("iint");
	opt_stat_t luk = row.get<opt_stat_t>("iluk");
	opt_health_t hp = row.get<opt_health_t>("ihp");
	opt_health_t mp = row.get<opt_health_t>("imp");
	opt_stat_t watk = row.get<opt_stat_t>("iwatk");
	opt_stat_t matk = row.get<opt_stat_t>("imatk");
	opt_stat_t wdef = row.get<opt_stat_t>("iwdef");
	opt_stat_t mdef = row.get<opt_stat_t>("imdef");
	opt_stat_t accuracy = row.get<opt_stat_t>("iacc");
	opt_stat_t avoid = row.get<opt_stat_t>("iavo");
	opt_stat_t hands = row.get<opt_stat_t>("ihand");
	opt_stat_t speed = row.get<opt_stat_t>("ispeed");
	opt_stat_t jump = row.get<opt_stat_t>("ijump");
	opt_int16_t flags = row.get<opt_int16_t>("flags");
	opt_int32_t hammers = row.get<opt_int32_t>("hammers");
	optional_t<pet_id_t> petId = row.get<optional_t<pet_id_t>>("pet_id");
	opt_int64_t expiration = row.get<opt_int64_t>("expiration");
	opt_string_t name = row.get<opt_string_t>("name");

	m_slots = slots.get(0);
	m_scrolls = scrolls.get(0);
	m_str = str.get(0);
	m_dex = dex.get(0);
	m_int = intt.get(0);
	m_luk = luk.get(0);
	m_hp = hp.get(0);
	m_mp = mp.get(0);
	m_wAtk = watk.get(0);
	m_mAtk = matk.get(0);
	m_wDef = wdef.get(0);
	m_mDef = mdef.get(0);
	m_accuracy = accuracy.get(0);
	m_avoid = avoid.get(0);
	m_hands = hands.get(0);
	m_speed = speed.get(0);
	m_jump = jump.get(0);
	m_hammers = hammers.get(0);
	m_flags = flags.get(0);
	m_expiration = expiration.is_initialized() ?
		FileTime{expiration.get()} :
		Items::NoExpiration;
	m_petId = petId.get(0);
	m_name = name.get("");
}

auto Item::databaseInsert(Database &db, const ItemDbInformation &info) -> void {
	vector_t<ItemDbRecord> v;
	ItemDbRecord r{info, this};
	v.push_back(r);
	Item::databaseInsert(db, v);
}

auto Item::databaseInsert(Database &db, const vector_t<ItemDbRecord> &items) -> void {
	using namespace soci;
	auto &sql = db.getSession();
	using MiscUtilities::getOptional;
	using MiscUtilities::NullableMode;

	static init_list_t<int8_t> nullsInt8 = {0};
	static init_list_t<int16_t> nullsInt16 = {0};
	static init_list_t<int32_t> nullsInt32 = {0};
	static init_list_t<int64_t> nullsInt64 = {0};
	static init_list_t<int64_t> nullsExpiration = {0, Items::NoExpiration.getValue()};
	static init_list_t<string_t> nullsString = {""};

	using opt_stat_t = optional_t<stat_t>;
	using opt_health_t = optional_t<health_t>;

	uint8_t inventory = 0;
	slot_qty_t amount = 0;
	item_id_t itemId = 0;
	inventory_slot_t slot = 0;
	world_id_t worldId = 0;
	account_id_t accountId = 0;
	player_id_t playerId = 0;
	string_t location = "";

	opt_int8_t slots;
	opt_int8_t scrolls;
	opt_stat_t iStr;
	opt_stat_t iDex;
	opt_stat_t iInt;
	opt_stat_t iLuk;
	opt_health_t iHp;
	opt_health_t iMp;
	opt_stat_t iWatk;
	opt_stat_t iMatk;
	opt_stat_t iWdef;
	opt_stat_t iMdef;
	opt_stat_t iAcc;
	opt_stat_t iAvo;
	opt_stat_t iHands;
	opt_stat_t iSpeed;
	opt_stat_t iJump;
	opt_int16_t flags;
	opt_int32_t hammers;
	optional_t<pet_id_t> petId;
	opt_int64_t expiration;
	opt_string_t name;

	statement st = (sql.prepare
		<< "INSERT INTO " << db.makeTable("items") << " (character_id, inv, slot, location, account_id, world_id, item_id, amount, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, flags, hammers, pet_id, name, expiration) "
		<< "VALUES (:char, :inv, :slot, :location, :account, :world, :itemid, :amount, :slots, :scrolls, :str, :dex, :int, :luk, :hp, :mp, :watk, :matk, :wdef, :mdef, :acc, :avo, :hands, :speed, :jump, :flags, :hammers, :pet, :name, :expiration)",
		use(playerId, "char"),
		use(inventory, "inv"),
		use(slot, "slot"),
		use(location, "location"),
		use(accountId, "account"),
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
		accountId = rec.userId;
		playerId = rec.charId;
		worldId = rec.worldId;
		slot = rec.slot;
		amount = item->m_amount;
		itemId = item->m_id;
		inventory = GameLogicUtilities::getInventory(itemId);
		bool equip = (inventory == Inventories::EquipInventory);
		NullableMode nulls = (equip ?
			NullableMode::NullIfFound :
			NullableMode::ForceNull);
		NullableMode required = (equip ?
			NullableMode::ForceNotNull :
			NullableMode::ForceNull);

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
		expiration = getOptional(item->m_expiration.getValue(), nulls, nullsExpiration);

		st.execute(true);
	}
}

}