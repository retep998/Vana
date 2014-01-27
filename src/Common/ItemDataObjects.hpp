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

struct SummonBag {
	int32_t mobId = 0;
	uint32_t chance = 0;
};

struct Skillbook {
	int32_t skillId = 0;
	uint8_t reqLevel = 0;
	uint8_t maxLevel = 0;
	int8_t chance = 0;
};

struct ScrollInfo {
	bool warmSupport = false;
	bool randStat = false;
	bool preventSlip = false;
	int8_t recover = 0;
	int16_t istr = 0;
	int16_t idex = 0;
	int16_t iint = 0;
	int16_t iluk = 0;
	int16_t ihp = 0;
	int16_t imp = 0;
	int16_t iwAtk = 0;
	int16_t imAtk = 0;
	int16_t iwDef = 0;
	int16_t imDef = 0;
	int16_t iacc = 0;
	int16_t iavo = 0;
	int16_t ihand = 0;
	int16_t ijump = 0;
	int16_t ispeed = 0;
	uint16_t success = 0;
	uint16_t cursed = 0;
};

struct Morph {
	int8_t chance = 100;
	int16_t morph = 0;
};

struct CardMapRange {
	int32_t startMap = 0;
	int32_t endMap = 0;
};

struct ConsumeInfo {
	bool autoConsume = false;
	bool ignoreWdef = false;
	bool ignoreMdef = false;
	bool party = false;
	bool mouseCancel = true;
	bool ignoreContinent = false;
	bool ghost = false;
	bool barrier = false;
	bool overrideTraction = false;
	bool preventDrown = false;
	bool preventFreeze = false;
	bool mesoUp = false;
	bool dropUp = false;
	bool partyDropUp = false;
	uint8_t effect = 0;
	uint8_t decHunger = 0;
	uint8_t decFatigue = 0;
	uint8_t cp = 0;
	int16_t hp = 0;
	int16_t mp = 0;
	int16_t hpr = 0;
	int16_t mpr = 0;
	int16_t wAtk = 0;
	int16_t mAtk = 0;
	int16_t avo = 0;
	int16_t acc = 0;
	int16_t wDef = 0;
	int16_t mDef = 0;
	int16_t speed = 0;
	int16_t jump = 0;
	int16_t fireResist = 0;
	int16_t iceResist = 0;
	int16_t lightningResist = 0;
	int16_t poisonResist = 0;
	int16_t curseDef = 0;
	int16_t stunDef = 0;
	int16_t weaknessDef = 0;
	int16_t darknessDef = 0;
	int16_t sealDef = 0;
	int16_t dropUpItemRange = 0;
	uint16_t mcProb = 0;
	int32_t dropUpItem = 0;
	int32_t moveTo = 0;
	int32_t time = 0;
	int32_t ailment = 0;
	vector_t<Morph> morphs;
	vector_t<CardMapRange> mapRanges;
};

struct ItemInfo {
	bool timeLimited = false;
	bool noSale = false;
	bool karmaScissors = false;
	bool expireOnLogout = false;
	bool blockPickup = false;
	bool noTrade = false;
	bool quest = false;
	bool cash = false;
	uint8_t minLevel = 0;
	uint8_t maxLevel = 0;
	uint8_t makerLevel = 0;
	uint16_t maxSlot = 0;
	int32_t price = 0;
	int32_t maxObtainable = 0;
	int32_t exp = 0;
	int32_t npc = 0;
	int32_t mesos = 0;
	string_t name;
};

struct PetInfo {
	bool noRevive = false;
	bool noStoringInCashShop = false;
	bool autoReact = false;
	int8_t evolveLevel = 0;
	int32_t hunger = 0;
	int32_t life = 0;
	int32_t limitedLife = 0;
	int32_t evolveItem = 0;
	string_t name;
};

struct PetInteractInfo {
	uint32_t prob = 0;
	int16_t increase = 0;
};

struct ItemRewardInfo {
	int32_t rewardId = 0;
	uint16_t prob = 0;
	int16_t quantity = 0;
	string_t effect;
};

struct EquipInfo {
	bool tradeBlockOnEquip = false;
	int8_t slots = 0;
	int8_t attackSpeed = 0;
	int8_t healing = 0;
	uint8_t tamingMob = 0;
	uint8_t iceDamage = 0;
	uint8_t fireDamage = 0;
	uint8_t lightningDamage = 0;
	uint8_t poisonDamage = 0;
	uint8_t elementalDefault = 0;
	int16_t istr = 0;
	int16_t idex = 0;
	int16_t iint = 0;
	int16_t iluk = 0;
	int16_t ihp = 0;
	int16_t imp = 0;
	int16_t iwAtk = 0;
	int16_t imAtk = 0;
	int16_t iwDef = 0;
	int16_t imDef = 0;
	int16_t iacc = 0;
	int16_t iavo = 0;
	int16_t ihand = 0;
	int16_t ijump = 0;
	int16_t ispeed = 0;
	int16_t reqStr = 0;
	int16_t reqDex = 0;
	int16_t reqInt = 0;
	int16_t reqLuk = 0;
	int16_t reqFame = 0;
	uint64_t validSlots = 0;
	double traction = 0.;
	vector_t<int8_t> validJobs;
};

struct DropInfo {
	bool isMesos = false;
	int32_t itemId = 0;
	int32_t minAmount = 0;
	int32_t maxAmount = 0;
	int16_t questId = 0;
	uint32_t chance = 0;
};

struct GlobalDrop {
	bool isMesos = false;
	int32_t itemId = 0;
	int32_t minAmount = 0;
	int32_t maxAmount = 0;
	uint8_t minLevel = 0;
	uint8_t maxLevel = 0;
	int8_t continent = -1;
	int16_t questId = 0;
	uint32_t chance = 0;
};