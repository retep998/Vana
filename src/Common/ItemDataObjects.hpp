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

#include "Types.hpp"
#include <string>
#include <vector>

struct SummonBag {
	mob_id_t mobId = 0;
	uint32_t chance = 0;
};

struct Skillbook {
	skill_id_t skillId = 0;
	skill_level_t reqLevel = 0;
	skill_level_t maxLevel = 0;
	int8_t chance = 0;
};

struct ScrollInfo {
	bool warmSupport = false;
	bool randStat = false;
	bool preventSlip = false;
	int8_t recover = 0;
	stat_t istr = 0;
	stat_t idex = 0;
	stat_t iint = 0;
	stat_t iluk = 0;
	health_t ihp = 0;
	health_t imp = 0;
	stat_t iwAtk = 0;
	stat_t imAtk = 0;
	stat_t iwDef = 0;
	stat_t imDef = 0;
	stat_t iacc = 0;
	stat_t iavo = 0;
	stat_t ihand = 0;
	stat_t ijump = 0;
	stat_t ispeed = 0;
	uint16_t success = 0;
	uint16_t cursed = 0;
};

struct Morph {
	int8_t chance = 100;
	morph_id_t morph = 0;
};

struct CardMapRange {
	map_id_t startMap = 0;
	map_id_t endMap = 0;
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
	health_t hp = 0;
	health_t mp = 0;
	int16_t hpr = 0;
	int16_t mpr = 0;
	stat_t wAtk = 0;
	stat_t mAtk = 0;
	stat_t avo = 0;
	stat_t acc = 0;
	stat_t wDef = 0;
	stat_t mDef = 0;
	stat_t speed = 0;
	stat_t jump = 0;
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
	item_id_t dropUpItem = 0;
	map_id_t moveTo = 0;
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
	player_level_t minLevel = 0;
	player_level_t maxLevel = 0;
	skill_level_t makerLevel = 0;
	slot_qty_t maxSlot = 0;
	mesos_t price = 0;
	int32_t maxObtainable = 0;
	experience_t exp = 0;
	npc_id_t npc = 0;
	mesos_t mesos = 0;
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
	item_id_t evolveItem = 0;
	string_t name;
};

struct PetInteractInfo {
	uint32_t prob = 0;
	int16_t increase = 0;
};

struct ItemRewardInfo {
	item_id_t rewardId = 0;
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
	stat_t istr = 0;
	stat_t idex = 0;
	stat_t iint = 0;
	stat_t iluk = 0;
	health_t ihp = 0;
	health_t imp = 0;
	stat_t iwAtk = 0;
	stat_t imAtk = 0;
	stat_t iwDef = 0;
	stat_t imDef = 0;
	stat_t iacc = 0;
	stat_t iavo = 0;
	stat_t ihand = 0;
	stat_t ijump = 0;
	stat_t ispeed = 0;
	stat_t reqStr = 0;
	stat_t reqDex = 0;
	stat_t reqInt = 0;
	stat_t reqLuk = 0;
	fame_t reqFame = 0;
	uint64_t validSlots = 0;
	double traction = 0.;
	vector_t<int8_t> validJobs;
};

struct DropInfo {
	bool isMesos = false;
	item_id_t itemId = 0;
	int32_t minAmount = 0;
	int32_t maxAmount = 0;
	quest_id_t questId = 0;
	uint32_t chance = 0;
};

struct GlobalDrop {
	bool isMesos = false;
	item_id_t itemId = 0;
	int32_t minAmount = 0;
	int32_t maxAmount = 0;
	player_level_t minLevel = 0;
	player_level_t maxLevel = 0;
	int8_t continent = -1;
	quest_id_t questId = 0;
	uint32_t chance = 0;
};