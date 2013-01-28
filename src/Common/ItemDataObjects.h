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
#pragma once

#include "Types.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

struct SummonBag {
	int32_t mobId;
	uint32_t chance;
};

struct Skillbook {
	int32_t skillId;
	uint8_t reqLevel;
	uint8_t maxLevel;
	int8_t chance;
};

struct ScrollInfo {
	ScrollInfo() : randStat(false), warmSupport(false), preventSlip(false), recover(0), ihand(0) { }
	bool warmSupport;
	bool randStat;
	bool preventSlip;
	int8_t recover;
	int16_t istr;
	int16_t idex;
	int16_t iint;
	int16_t iluk;
	int16_t ihp;
	int16_t imp;
	int16_t iwatk;
	int16_t imatk;
	int16_t iwdef;
	int16_t imdef;
	int16_t iacc;
	int16_t iavo;
	int16_t ihand;
	int16_t ijump;
	int16_t ispeed;
	uint16_t success;
	uint16_t cursed;
};

struct Morph {
	Morph() : chance(100) { }
	int8_t chance;
	int16_t morph;
};

struct CardMapRange {
	int32_t startMap;
	int32_t endMap;
};

struct ConsumeInfo {
	ConsumeInfo() :
		fireResist(0), iceResist(0), lightningResist(0), poisonResist(0),
		curseDef(0), stunDef(0), weaknessDef(0), darknessDef(0), sealDef(0),
		dropUpItem(0), dropUpItemRange(0),
		ailment(0),
		autoConsume(false), ignoreWdef(false), ignoreMdef(false), party(false), mouseCancel(true),
		ignoreContinent(false), ghost(false), barrier(false), overrideTraction(false), preventDrown(false),
		preventFreeze(false), mesoUp(false), partyDropUp(false), dropUp(false)
		{ }

	bool autoConsume;
	bool ignoreWdef;
	bool ignoreMdef;
	bool party;
	bool mouseCancel;
	bool ignoreContinent;
	bool ghost;
	bool barrier;
	bool overrideTraction;
	bool preventDrown;
	bool preventFreeze;
	bool mesoUp;
	bool dropUp;
	bool partyDropUp;
	uint8_t effect;
	uint8_t decHunger;
	uint8_t decFatigue;
	uint8_t cp;
	int16_t hp;
	int16_t mp;
	int16_t hpr;
	int16_t mpr;
	int16_t watk;
	int16_t matk;
	int16_t avo;
	int16_t acc;
	int16_t wdef;
	int16_t mdef;
	int16_t speed;
	int16_t jump;
	int16_t fireResist;
	int16_t iceResist;
	int16_t lightningResist;
	int16_t poisonResist;
	int16_t curseDef;
	int16_t stunDef;
	int16_t weaknessDef;
	int16_t darknessDef;
	int16_t sealDef;
	int16_t dropUpItemRange;
	uint16_t mcProb;
	int32_t dropUpItem;
	int32_t moveTo;
	int32_t time;
	int32_t ailment;
	vector<Morph> morphs;
	vector<CardMapRange> mapRanges;
};

struct ItemInfo {
	ItemInfo() :
		timeLimited(false),
		noSale(false),
		karmaScissors(false),
		expireOnLogout(false),
		blockPickup(false),
		noTrade(false),
		cash(false),
		quest(false)
		{ }

	string name;
	uint8_t minLevel;
	uint8_t maxLevel;
	uint8_t makerLevel;
	uint16_t maxSlot;
	int32_t price;
	int32_t maxObtainable;
	int32_t exp;
	int32_t npc;
	int32_t mesos;
	bool timeLimited;
	bool noSale;
	bool karmaScissors;
	bool expireOnLogout;
	bool blockPickup;
	bool noTrade;
	bool quest;
	bool cash;
};

struct PetInfo {
	PetInfo() : noRevive(false), noStoringInCashShop(false), autoReact(false) { }
	bool noRevive;
	bool noStoringInCashShop;
	bool autoReact;
	int8_t evolveLevel;
	int32_t hunger;
	int32_t life;
	int32_t limitedLife;
	int32_t evolveItem;
	string name;
};

struct PetInteractInfo {
	uint32_t prob;
	int16_t increase;
};

struct ItemRewardInfo {
	int32_t rewardId;
	uint16_t prob;
	int16_t quantity;
	string effect;
};

struct EquipInfo {
	EquipInfo() : tradeBlockOnEquip(false), validSlots(0) { }

	bool tradeBlockOnEquip;
	int8_t slots;
	int8_t attackSpeed;
	int8_t healing;
	uint8_t tamingMob;
	uint8_t iceDamage;
	uint8_t fireDamage;
	uint8_t lightningDamage;
	uint8_t poisonDamage;
	uint8_t elementalDefault;
	int16_t istr;
	int16_t idex;
	int16_t iint;
	int16_t iluk;
	int16_t ihp;
	int16_t imp;
	int16_t iwatk;
	int16_t imatk;
	int16_t iwdef;
	int16_t imdef;
	int16_t iacc;
	int16_t iavo;
	int16_t ihand;
	int16_t ijump;
	int16_t ispeed;
	int16_t reqStr;
	int16_t reqDex;
	int16_t reqInt;
	int16_t reqLuk;
	int16_t reqFame;
	uint64_t validSlots;
	vector<int8_t> validJobs;
	double traction;
};

struct DropInfo {
	DropInfo() : isMesos(false), itemId(0), minAmount(0), maxAmount(0), questId(0), chance(0) { }
	bool isMesos;
	int32_t itemId;
	int32_t minAmount;
	int32_t maxAmount;
	int16_t questId;
	uint32_t chance;
};
typedef vector<DropInfo> DropsInfo;

struct GlobalDrop {
	GlobalDrop() : isMesos(false), itemId(0), minAmount(0), maxAmount(0), minLevel(1), maxLevel(200), continent(-1), questId(0), chance(0) { }
	bool isMesos;
	int32_t itemId;
	int32_t minAmount;
	int32_t maxAmount;
	uint8_t minLevel;
	uint8_t maxLevel;
	int8_t continent;
	int16_t questId;
	uint32_t chance;
};
typedef vector<GlobalDrop> GlobalDrops;