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
#include "Item.h"
#include "EquipDataProvider.h"
#include "ItemConstants.h"

Item::Item() { }

// The only places that use this constructor set everything manually
Item::Item(int32_t itemid) :
m_id(itemid)
{
}

//
Item::Item(int32_t itemid, int16_t amount) :
m_id(itemid),
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
m_acc(0),
m_avo(0),
m_hands(0),
m_jump(0),
m_speed(0),
m_petId(0),
m_cashId(0),
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
m_cashId(0),
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
	m_acc = item->getAccuracy();
	m_avo = item->getAvoid();
	m_hands = item->getHands();
	m_jump = item->getJump();
	m_speed = item->getSpeed();
	m_petId = item->getPetId();
	m_name = item->getName();
	m_cashId = item->getCashId();
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
	m_acc = testStat(acc, Items::MaxStats::Acc);
}

void Item::setAvoid(int16_t avoid) {
	m_avo = testStat(avoid, Items::MaxStats::Avoid);
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
	int16_t ret = stat;
	if (ret < 0) {
		ret = 0;
	}
	else if (ret > max) {
		ret = max;
	}
	return ret;
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
	if (onlyIfExists && stat == 0) {
		return false;
	}
	return true;
}