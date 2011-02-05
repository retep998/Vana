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

class Item {
public:
	Item();
	Item(int32_t itemid);
	Item(int32_t itemid, int16_t amount);
	Item(int32_t equipid, bool random);
	Item(Item *item);

	bool hasWarmSupport() const;
	bool hasSlipPrevention() const;
	bool hasLock() const;
	bool hasKarma() const;
	bool hasTradeBlock() const;
	int8_t getSlots() const { return m_slots; }
	int8_t getScrolls() const { return m_scrolls; }
	int16_t getStr() const { return m_str; }
	int16_t getDex() const { return m_dex; }
	int16_t getInt() const { return m_int; }
	int16_t getLuk() const { return m_luk; }
	int16_t getHp() const { return m_hp; }
	int16_t getMp() const { return m_mp; }
	int16_t getWatk() const { return m_watk; }
	int16_t getMatk() const { return m_matk; }
	int16_t getWdef() const { return m_wdef; }
	int16_t getMdef() const { return m_mdef; }
	int16_t getAccuracy() const { return m_acc; }
	int16_t getAvoid() const { return m_avo; }
	int16_t getHands() const { return m_hands; }
	int16_t getSpeed() const { return m_speed; }
	int16_t getJump() const { return m_jump; }
	int16_t getAmount() const { return m_amount; }
	int16_t getFlags() const { return m_flags; }
	int32_t getId() const { return m_id; }
	int32_t getHammers() const { return m_hammers; }
	int32_t getPetId() const { return m_petId; }
	int64_t getCashId() const { return m_cashId; }
	int64_t getExpirationTime() const { return m_expiration; }
	const string & getName() const { return m_name; }

	void setSlots(int8_t slots) { m_slots = slots; }
	void setScrolls(int8_t scrolls) { m_scrolls = scrolls; }
	void setStr(int16_t strength);
	void setDex(int16_t dexterity);
	void setInt(int16_t intelligence);
	void setLuk(int16_t luck);
	void setHp(int16_t hp);
	void setMp(int16_t mp);
	void setWatk(int16_t watk);
	void setWdef(int16_t wdef);
	void setMatk(int16_t matk);
	void setMdef(int16_t mdef);
	void setAccuracy(int16_t acc);
	void setAvoid(int16_t avoid);
	void setHands(int16_t hands);
	void setJump(int16_t jump);
	void setSpeed(int16_t speed);
	void setPetId(int32_t petId) { m_petId = petId; }
	void setAmount(int16_t amount) { m_amount = amount; }
	void setName(const string &name) { m_name = name; }
	void setFlags(int16_t flags) { m_flags = flags; }
	void setHammers(int32_t hammers) { m_hammers = hammers; }
	void setPreventSlip(bool prevent);
	void setWarmSupport(bool warm);
	void setLock(bool lock);
	void setKarma(bool karma);
	void setTradeBlock(bool block);
	void setCashId(int64_t id) { m_cashId = id; }
	void setExpirationTime(int64_t time) { m_expiration = time; }
	void addStr(int16_t strength, bool onlyIfExists = false);
	void addDex(int16_t dexterity, bool onlyIfExists = false);
	void addInt(int16_t intelligence, bool onlyIfExists = false);
	void addLuk(int16_t luck, bool onlyIfExists = false);
	void addHp(int16_t hp, bool onlyIfExists = false);
	void addMp(int16_t mp, bool onlyIfExists = false);
	void addWatk(int16_t watk, bool onlyIfExists = false);
	void addWdef(int16_t wdef, bool onlyIfExists = false);
	void addMatk(int16_t matk, bool onlyIfExists = false);
	void addMdef(int16_t mdef, bool onlyIfExists = false);
	void addAccuracy(int16_t acc, bool onlyIfExists = false);
	void addAvoid(int16_t avoid, bool onlyIfExists = false);
	void addHands(int16_t hands, bool onlyIfExists = false);
	void addJump(int16_t jump, bool onlyIfExists = false);
	void addSpeed(int16_t speed, bool onlyIfExists = false);
	void incAmount(int16_t mod) { m_amount += mod; }
	void decAmount(int16_t mod) { m_amount -= mod; }
	void incHammers() { m_hammers++; }
	void incSlots() { m_slots++; }
	void decSlots() { m_slots--; }
	void incScrolls() { m_scrolls++; }
private:
	bool testPerform(int16_t stat, bool onlyIfExists);
	int16_t testStat(int16_t stat, int16_t max);
	void modifyFlags(bool add, int16_t flags);
	bool testFlags(int16_t flags) const;

	int8_t m_slots;
	int8_t m_scrolls;
	int16_t m_str;
	int16_t m_dex;
	int16_t m_int;
	int16_t m_luk;
	int16_t m_hp;
	int16_t m_mp;
	int16_t m_watk;
	int16_t m_matk;
	int16_t m_wdef;
	int16_t m_mdef;
	int16_t m_acc;
	int16_t m_avo;
	int16_t m_hands;
	int16_t m_jump;
	int16_t m_speed;
	int16_t m_flags;
	int16_t m_amount;
	int32_t m_id;
	int32_t m_hammers;
	int32_t m_petId;
	int64_t m_cashId;
	int64_t m_expiration;
	string m_name;
};