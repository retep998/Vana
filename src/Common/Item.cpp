/*
Copyright (C) 2008-2013 Vana Development Team

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
#include "Item.h"
#include "EquipDataProvider.h"
#include "GameLogicUtilities.h"
#include "ItemConstants.h"
#include "MiscUtilities.h"
#include "SociExtensions.h"
#include <soci.h>

const string Item::Inventory = "inventory";
const string Item::Storage = "storage";

Item::Item() { }

// The only places that use this constructor set everything manually
Item::Item(const soci::row &row)
{
	initializeItem(row);
}

Item::Item(int32_t itemId, int16_t amount) :
	m_id(itemId),
	m_amount(amount),
	m_hammers(0),
	m_slots(0),
	m_scrolls(0),
	m_flags(0),
	m_str(0),
	m_dex(0),
	m_int(0),
	m_luk(0),
	m_hp(0),
	m_mp(0),
	m_watk(0),
	m_matk(0),
	m_wdef(0),
	m_mdef(0),
	m_accuracy(0),
	m_avoid(0),
	m_hands(0),
	m_jump(0),
	m_speed(0),
	m_petId(0),
	m_expiration(Items::NoExpiration)
{
}

Item::Item(int32_t equipid, bool random) :
	m_id(equipid),
	m_amount(1),
	m_scrolls(0),
	m_hammers(0),
	m_flags(0),
	m_petId(0),
	m_expiration(Items::NoExpiration)
{
	EquipDataProvider::Instance()->setEquipStats(this, random);
}

Item::Item(Item *item) {
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
	m_watk = item->getWatk();
	m_matk = item->getMatk();
	m_wdef = item->getWdef();
	m_mdef = item->getMdef();
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

bool Item::hasSlipPrevention() const {
	return testFlags(Items::Flags::Spikes);
}

bool Item::hasWarmSupport() const {
	return testFlags(Items::Flags::ColdProtection);
}

bool Item::hasLock() const {
	return testFlags(Items::Flags::Lock);
}

bool Item::hasKarma() const {
	return testFlags(Items::Flags::KarmaScissors);
}

bool Item::hasTradeBlock() const {
	return testFlags(Items::Flags::TradeUnavailable);
}

bool Item::testFlags(int16_t flags) const {
	return ((m_flags & flags) != 0);
}

void Item::setPreventSlip(bool prevent) {
	modifyFlags(prevent, Items::Flags::Spikes);
}

void Item::setWarmSupport(bool warm) {
	modifyFlags(warm, Items::Flags::ColdProtection);
}

void Item::setLock(bool lock) {
	modifyFlags(lock, Items::Flags::Lock);
}

void Item::setKarma(bool karma) {
	modifyFlags(karma, Items::Flags::KarmaScissors);
}

void Item::setTradeBlock(bool block) {
	modifyFlags(block, Items::Flags::TradeUnavailable);
}

void Item::modifyFlags(bool add, int16_t flags) {
	if (add) {
		m_flags |= flags;
	}
	else {
		m_flags &= ~(flags);
	}
}

void Item::setStr(int16_t strength) {
	m_str = testStat(strength, Items::MaxStats::Str);
}

void Item::setDex(int16_t dexterity) {
	m_dex = testStat(dexterity, Items::MaxStats::Dex);
}

void Item::setInt(int16_t intelligence) {
	m_int = testStat(intelligence, Items::MaxStats::Int);
}

void Item::setLuk(int16_t luck) {
	m_luk = testStat(luck, Items::MaxStats::Luk);
}

void Item::setHp(int16_t hp) {
	m_hp = testStat(hp, Items::MaxStats::Hp);
}

void Item::setMp(int16_t mp) {
	m_mp = testStat(mp, Items::MaxStats::Mp);
}

void Item::setWatk(int16_t watk) {
	m_watk = testStat(watk, Items::MaxStats::Watk);
}

void Item::setWdef(int16_t wdef) {
	m_wdef = testStat(wdef, Items::MaxStats::Wdef);
}

void Item::setMatk(int16_t matk) {
	m_matk = testStat(matk, Items::MaxStats::Matk);
}

void Item::setMdef(int16_t mdef) {
	m_mdef = testStat(mdef, Items::MaxStats::Mdef);
}

void Item::setAccuracy(int16_t acc) {
	m_accuracy = testStat(acc, Items::MaxStats::Acc);
}

void Item::setAvoid(int16_t avoid) {
	m_avoid = testStat(avoid, Items::MaxStats::Avoid);
}

void Item::setHands(int16_t hands) {
	m_hands = testStat(hands, Items::MaxStats::Hands);
}

void Item::setJump(int16_t jump) {
	m_jump = testStat(jump, Items::MaxStats::Jump);
}

void Item::setSpeed(int16_t speed) {
	m_speed = testStat(speed, Items::MaxStats::Speed);
}

int16_t Item::testStat(int16_t stat, int16_t max) {
	return (stat < 0 ? 0 : (stat > max ? max : stat));
}

void Item::addStr(int16_t strength, bool onlyIfExists) {
	if (testPerform(getStr(), onlyIfExists)) {
		setStr(getStr() + strength);
	}
}

void Item::addDex(int16_t dexterity, bool onlyIfExists) {
	if (testPerform(getDex(), onlyIfExists)) {
		setDex(getDex() + dexterity);
	}
}

void Item::addInt(int16_t intelligence, bool onlyIfExists) {
	if (testPerform(getInt(), onlyIfExists)) {
		setInt(getInt() + intelligence);
	}
}

void Item::addLuk(int16_t luck, bool onlyIfExists) {
	if (testPerform(getLuk(), onlyIfExists)) {
		setLuk(getLuk() + luck);
	}
}

void Item::addHp(int16_t hp, bool onlyIfExists) {
	if (testPerform(getHp(), onlyIfExists)) {
		setHp(getHp() + hp);
	}
}

void Item::addMp(int16_t mp, bool onlyIfExists) {
	if (testPerform(getMp(), onlyIfExists)) {
		setMp(getMp() + mp);
	}
}

void Item::addWatk(int16_t watk, bool onlyIfExists) {
	if (testPerform(getWatk(), onlyIfExists)) {
		setWatk(getWatk() + watk);
	}
}

void Item::addWdef(int16_t wdef, bool onlyIfExists) {
	if (testPerform(getWdef(), onlyIfExists)) {
		setWdef(getWdef() + wdef);
	}
}

void Item::addMatk(int16_t matk, bool onlyIfExists) {
	if (testPerform(getMatk(), onlyIfExists)) {
		setMatk(getMatk() + matk);
	}
}

void Item::addMdef(int16_t mdef, bool onlyIfExists) {
	if (testPerform(getMdef(), onlyIfExists)) {
		setMdef(getMdef() + mdef);
	}
}

void Item::addAccuracy(int16_t acc, bool onlyIfExists) {
	if (testPerform(getAccuracy(), onlyIfExists)) {
		setAccuracy(getAccuracy() + acc);
	}
}

void Item::addAvoid(int16_t avoid, bool onlyIfExists) {
	if (testPerform(getAvoid(), onlyIfExists)) {
		setAvoid(getAvoid() + avoid);
	}
}

void Item::addHands(int16_t hands, bool onlyIfExists) {
	if (testPerform(getHands(), onlyIfExists)) {
		setHands(getHands() + hands);
	}
}

void Item::addJump(int16_t jump, bool onlyIfExists) {
	if (testPerform(getJump(), onlyIfExists)) {
		setJump(getJump() + jump);
	}
}

void Item::addSpeed(int16_t speed, bool onlyIfExists) {
	if (testPerform(getSpeed(), onlyIfExists)) {
		setSpeed(getSpeed() + speed);
	}
}

bool Item::testPerform(int16_t stat, bool onlyIfExists) {
	return !onlyIfExists || stat != 0;
}

void Item::setAmount(int16_t amount) {
	m_amount = amount;
}

void Item::setSlots(int8_t slots) {
	m_slots = slots;
}

void Item::setPetId(int64_t petId) {
	m_petId = petId;
}

void Item::setName(const string &name) {
	m_name = name;
}

void Item::initializeItem(const soci::row &row) {
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
	opt_string name = row.get<opt_string>("name");

	m_slots = slots.is_initialized() ? slots.get() : 0;
	m_scrolls = scrolls.is_initialized() ? scrolls.get() : 0;
	m_str = str.is_initialized() ? str.get() : 0;
	m_dex = dex.is_initialized() ? dex.get() : 0;
	m_int = intt.is_initialized() ? intt.get() : 0;
	m_luk = luk.is_initialized() ? luk.get() : 0;
	m_hp = hp.is_initialized() ? hp.get() : 0;
	m_mp = mp.is_initialized() ? mp.get() : 0;
	m_watk = watk.is_initialized() ? watk.get() : 0;
	m_matk = matk.is_initialized() ? matk.get() : 0;
	m_wdef = wdef.is_initialized() ? wdef.get() : 0;
	m_mdef = mdef.is_initialized() ? mdef.get() : 0;
	m_accuracy = accuracy.is_initialized() ? accuracy.get() : 0;
	m_avoid = avoid.is_initialized() ? avoid.get() : 0;
	m_hands = hands.is_initialized() ? hands.get() : 0;
	m_speed = speed.is_initialized() ? speed.get() : 0;
	m_jump = jump.is_initialized() ? jump.get() : 0;
	m_hammers = hammers.is_initialized() ? hammers.get() : 0;
	m_flags = flags.is_initialized() ? flags.get() : 0;
	m_expiration = expiration.is_initialized() ? expiration.get() : Items::NoExpiration;
	m_petId = petId.is_initialized() ? petId.get() : 0;
	m_name = name.is_initialized() ? name.get() : "";
}

void Item::databaseInsert(soci::session &sql, const ItemDbInformation &info) {
	vector<ItemDbRecord> v;
	ItemDbRecord r(info, this);
	v.push_back(r);
	Item::databaseInsert(sql, v);
}

void Item::databaseInsert(soci::session &sql, const vector<ItemDbRecord> &items) {
	using namespace soci;
	using MiscUtilities::NullableMode;
	using MiscUtilities::getOptional;

	static int8_t nullsInt8[] = {0};
	static int16_t nullsInt16[] = {0};
	static int32_t nullsInt32[] = {0};
	static int64_t nullsInt64[] = {0};
	static int64_t nullsExpiration[] = {0, Items::NoExpiration};
	static string nullsString[] = {""};

	uint8_t inventory = 0;
	int16_t amount = 0;
	int32_t itemId = 0;
	int16_t slot = 0;
	int8_t worldId = 0;
	int32_t userId = 0;
	int32_t playerId = 0;
	string location = "";

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
	opt_string name;

	statement st = (sql.prepare
		<< "INSERT INTO items (character_id, inv, slot, location, user_id, world_id, item_id, amount, slots, scrolls, istr, idex, iint, iluk, ihp, imp, iwatk, imatk, iwdef, imdef, iacc, iavo, ihand, ispeed, ijump, flags, hammers, pet_id, name, expiration) "
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

	for (auto iter = items.begin(); iter != items.end() ; ++iter) {
		const ItemDbRecord &rec = *iter;
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
		NullableMode nulls = (equip ? MiscUtilities::NullIfFound : MiscUtilities::ForceNull);
		NullableMode required = (equip ? MiscUtilities::ForceNotNull : MiscUtilities::ForceNull);

		slots = getOptional(item->m_slots, required, nullsInt8);
		scrolls = getOptional(item->m_scrolls, required, nullsInt8);
		iStr = getOptional(item->m_str, nulls, nullsInt16);
		iDex = getOptional(item->m_dex, nulls, nullsInt16);
		iInt = getOptional(item->m_int, nulls, nullsInt16);
		iLuk = getOptional(item->m_luk, nulls, nullsInt16);
		iHp = getOptional(item->m_hp, nulls, nullsInt16);
		iMp = getOptional(item->m_mp, nulls, nullsInt16);
		iWatk = getOptional(item->m_watk, nulls, nullsInt16);
		iMatk = getOptional(item->m_matk, nulls, nullsInt16);
		iWdef = getOptional(item->m_wdef, nulls, nullsInt16);
		iMdef = getOptional(item->m_mdef, nulls, nullsInt16);
		iAcc = getOptional(item->m_accuracy, nulls, nullsInt16);
		iAvo = getOptional(item->m_avoid, nulls, nullsInt16);
		iHands = getOptional(item->m_hands, nulls, nullsInt16);
		iSpeed = getOptional(item->m_speed, nulls, nullsInt16);
		iJump = getOptional(item->m_jump, nulls, nullsInt16);
		flags = getOptional(item->m_flags, nulls, nullsInt16);
		hammers = getOptional(item->m_hammers, nulls, nullsInt32);
		petId = getOptional(item->m_petId, nulls, nullsInt64);
		name = getOptional(item->m_name, nulls, nullsString);
		expiration = getOptional(item->m_expiration, nulls, nullsExpiration, 2);

		st.execute(true);
	}
}