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
#include "ItemDataProvider.hpp"
#include "Algorithm.hpp"
#include "BuffDataProvider.hpp"
#include "Database.hpp"
#include "EquipDataProvider.hpp"
#include "GameConstants.hpp"
#include "GameLogicUtilities.hpp"
#include "GameObjects.hpp"
#include "InitializeCommon.hpp"
#include "Randomizer.hpp"
#include "ShopDataProvider.hpp"
#include "StringUtilities.hpp"
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

auto ItemDataProvider::loadData() -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Items... ";

	loadItems();
	loadConsumes();
	loadMultiMorphs();
	loadMapRanges();
	loadScrolls();
	loadMonsterCardData();
	loadItemSkills();
	loadSummonBags();
	loadItemRewards();
	loadPets();
	loadPetInteractions();

	std::cout << "DONE" << std::endl;
}

auto ItemDataProvider::loadItems() -> void {
	m_itemInfo.clear();
	int32_t itemId;
	ItemInfo item;

	soci::rowset<> rs = (Database::getDataDb().prepare
		<< "SELECT id.*, s.label "
		<< "FROM item_data id "
		<< "LEFT JOIN strings s ON id.itemId = s.objectid AND s.object_type = :item",
		soci::use(string_t("item"), "item"));

	for (const auto &row : rs) {
		item = ItemInfo();
		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&item](const string_t &cmp) {
			if (cmp == "time_limited") item.timeLimited = true;
			else if (cmp == "cash_item") item.cash = true;
			else if (cmp == "no_trade") item.noTrade = true;
			else if (cmp == "no_sale") item.noSale = true;
			else if (cmp == "karma_scissorable") item.karmaScissors = true;
			else if (cmp == "expire_on_logout") item.expireOnLogout = true;
			else if (cmp == "block_pickup") item.blockPickup = true;
			else if (cmp == "quest") item.quest = true;
		});

		itemId = row.get<int32_t>("itemid");
		item.price = row.get<int32_t>("price");
		item.maxSlot = row.get<uint16_t>("max_slot_quantity");
		item.makerLevel = row.get<uint8_t>("level_for_maker");
		item.maxObtainable = row.get<int32_t>("max_possession_count");
		item.minLevel = row.get<uint8_t>("min_level");
		item.maxLevel = row.get<uint8_t>("max_level");
		item.exp = row.get<int32_t>("experience");
		item.mesos = row.get<int32_t>("money");
		item.npc = row.get<int32_t>("npc");
		item.name = row.get<string_t>("label");

		m_itemInfo[itemId] = item;
	}
}

auto ItemDataProvider::loadScrolls() -> void {
	m_scrollInfo.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_scroll_data");

	for (const auto &row : rs) {
		ScrollInfo item;
		int32_t itemId = row.get<int32_t>("itemid");
		item.success = row.get<uint16_t>("success");
		item.cursed = row.get<uint16_t>("break_item");
		item.istr = row.get<int16_t>("istr");
		item.idex = row.get<int16_t>("idex");
		item.iint = row.get<int16_t>("iint");
		item.iluk = row.get<int16_t>("iluk");
		item.ihp = row.get<int16_t>("ihp");
		item.imp = row.get<int16_t>("imp");
		item.iwAtk = row.get<int16_t>("iwatk");
		item.imAtk = row.get<int16_t>("imatk");
		item.iwDef = row.get<int16_t>("iwdef");
		item.imDef = row.get<int16_t>("imdef");
		item.iacc = row.get<int16_t>("iacc");
		item.iavo = row.get<int16_t>("iavo");
		item.ijump = row.get<int16_t>("ijump");
		item.ispeed = row.get<int16_t>("ispeed");

		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&item](const string_t &cmp) {
			if (cmp == "rand_stat") item.randStat = true;
			else if (cmp == "recover_slot") item.recover = 1;
			else if (cmp == "warm_support") item.warmSupport = true;
			else if (cmp == "prevent_slip") item.preventSlip = true;
		});

		m_scrollInfo[itemId] = item;
	}
}

auto ItemDataProvider::loadConsumes() -> void {
	m_consumeInfo.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_consume_data");

	for (const auto &row : rs) {
		ConsumeInfo item;
		int32_t itemId = row.get<int32_t>("itemid");
		item.effect = row.get<uint8_t>("effect");
		item.hp = row.get<int16_t>("hp");
		item.mp = row.get<int16_t>("mp");
		item.hpr = row.get<int16_t>("hp_percentage");
		item.mpr = row.get<int16_t>("mp_percentage");
		item.moveTo = row.get<int32_t>("move_to");
		item.decHunger = row.get<uint8_t>("decrease_hunger");
		item.decFatigue = row.get<uint8_t>("decrease_fatigue");
		item.cp = row.get<uint8_t>("carnival_points");
		item.mcProb = row.get<uint16_t>("prob");
		item.time = row.get<int32_t>("buff_time");
		item.wAtk = row.get<int16_t>("weapon_attack");
		item.mAtk = row.get<int16_t>("magic_attack");
		item.wDef = row.get<int16_t>("weapon_defense");
		item.mDef = row.get<int16_t>("magic_defense");
		item.acc = row.get<int16_t>("accuracy");
		item.avo = row.get<int16_t>("avoid");
		item.speed = row.get<int16_t>("speed");
		item.jump = row.get<int16_t>("jump");

		int16_t morphId = row.get<int16_t>("morph");
		if (morphId != 0) {
			Morph morph;
			morph.morph = morphId;
			morph.chance = 100;
			item.morphs.push_back(morph);
		}

		item.iceResist = row.get<int16_t>("defense_vs_ice");
		item.fireResist = row.get<int16_t>("defense_vs_fire");
		item.lightningResist = row.get<int16_t>("defense_vs_lightning");
		item.poisonResist = row.get<int16_t>("defense_vs_poison");
		item.stunDef = row.get<int16_t>("defense_vs_stun");
		item.darknessDef = row.get<int16_t>("defense_vs_darkness");
		item.weaknessDef = row.get<int16_t>("defense_vs_weakness");
		item.sealDef = row.get<int16_t>("defense_vs_seal");
		item.curseDef = row.get<int16_t>("defense_vs_curse");

		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&item](const string_t &cmp) {
			if (cmp == "auto_consume") item.autoConsume = true;
			else if (cmp == "party_item") item.party = true;
			else if (cmp == "meso_up") item.mesoUp = true;
			else if (cmp == "ignore_physical_defense") item.ignoreWdef = true;
			else if (cmp == "ignore_magical_defense") item.ignoreMdef = true;
			else if (cmp == "no_mouse_cancel") item.mouseCancel = false;
			else if (cmp == "ignore_continent") item.ignoreContinent = true;
			else if (cmp == "ghost") item.ghost = true;
			else if (cmp == "barrier") item.barrier = true;
			else if (cmp == "prevent_drowning") item.preventDrown = true;
			else if (cmp == "prevent_freezing") item.preventFreeze = true;
			else if (cmp == "override_traction") item.overrideTraction = true;
			else if (cmp == "drop_up_for_party") item.partyDropUp = true;
		});

		StringUtilities::runFlags(row.get<opt_string_t>("drop_up"), [&item, &row](const string_t &cmp) {
			if (cmp == "none") return;

			item.dropUp = true;
			if (cmp == "specific_item") item.dropUpItem = row.get<int32_t>("drop_up_item");
			else if (cmp == "item_range") item.dropUpItemRange = row.get<int16_t>("drop_up_item_range");
		});

		StringUtilities::runFlags(row.get<opt_string_t>("cure_ailments"), [&item](const string_t &cmp) {
			if (cmp == "darkness") item.ailment |= 0x01;
			else if (cmp == "poison") item.ailment |= 0x02;
			else if (cmp == "curse") item.ailment |= 0x04;
			else if (cmp == "seal") item.ailment |= 0x08;
			else if (cmp == "weakness") item.ailment |= 0x10;
		});

		BuffDataProvider::getInstance().addItemInfo(itemId, item);
		m_consumeInfo[itemId] = item;
	}
}

auto ItemDataProvider::loadMapRanges() -> void {
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_monster_card_map_ranges");

	for (const auto &row : rs) {
		CardMapRange range;
		int32_t itemId = row.get<int32_t>("itemid");
		range.startMap = row.get<int32_t>("start_map");
		range.endMap = row.get<int32_t>("end_map");

		m_consumeInfo[itemId].mapRanges.push_back(range);
	}
}

auto ItemDataProvider::loadMultiMorphs() -> void {
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_random_morphs");

	for (const auto &row : rs) {
		Morph morph;
		int32_t itemId = row.get<int32_t>("itemid");
		morph.morph = row.get<int16_t>("morphid");
		morph.chance = row.get<int8_t>("success");

		m_consumeInfo[itemId].morphs.push_back(morph);
	}
}

auto ItemDataProvider::loadMonsterCardData() -> void {
	m_cardsToMobs.clear();
	m_mobsToCards.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM monster_card_data");

	for (const auto &row : rs) {
		int32_t cardId = row.get<int32_t>("cardid");
		int32_t mobId = row.get<int32_t>("mobid");

		m_cardsToMobs.emplace(cardId, mobId);
		m_mobsToCards.emplace(mobId, cardId);
	}
}

auto ItemDataProvider::loadItemSkills() -> void {
	m_skillbooks.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_skills");

	for (const auto &row : rs) {
		Skillbook skill;
		int32_t itemId = row.get<int32_t>("itemid");
		skill.skillId = row.get<int32_t>("skillid");
		skill.reqLevel = row.get<uint8_t>("req_skill_level");
		skill.maxLevel = row.get<uint8_t>("master_level");
		skill.chance = row.get<int8_t>("chance");

		m_skillbooks[itemId].push_back(skill);
	}
}

auto ItemDataProvider::loadSummonBags() -> void {
	m_summonBags.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_summons");

	for (const auto &row : rs) {
		SummonBag summon;
		int32_t itemId = row.get<int32_t>("itemid");
		summon.mobId = row.get<int32_t>("mobid");
		summon.chance = row.get<uint16_t>("chance");

		m_summonBags[itemId].push_back(summon);
	}
}

auto ItemDataProvider::loadItemRewards() -> void {
	m_itemRewards.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_reward_data");

	for (const auto &row : rs) {
		ItemRewardInfo reward;
		int32_t itemId = row.get<int32_t>("itemid");
		reward.rewardId = row.get<int32_t>("rewardid");
		reward.prob = row.get<uint16_t>("prob");
		reward.quantity = row.get<int16_t>("quantity");
		reward.effect = row.get<string_t>("effect");

		m_itemRewards[itemId].push_back(reward);
	}
}

auto ItemDataProvider::loadPets() -> void {
	m_petInfo.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_pet_data");

	for (const auto &row : rs) {
		PetInfo pet;
		int32_t itemId = row.get<int32_t>("itemid");
		pet.name = row.get<string_t>("default_name");
		pet.hunger = row.get<int32_t>("hunger");
		pet.life = row.get<int32_t>("life");
		pet.limitedLife = row.get<int32_t>("limited_life");
		pet.evolveItem = row.get<int32_t>("evolution_item");
		pet.evolveLevel = row.get<int8_t>("req_level_for_evolution");
		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&pet](const string_t &cmp) {
			if (cmp == "no_revive") pet.noRevive = true;
			else if (cmp == "no_move_to_cash_shop") pet.noStoringInCashShop = true;
			else if (cmp == "auto_react") pet.autoReact = true;
		});

		m_petInfo[itemId] = pet;
	}
}

auto ItemDataProvider::loadPetInteractions() -> void {
	m_petInteractInfo.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_pet_interactions");

	for (const auto &row : rs) {
		PetInteractInfo interaction;
		int32_t itemId = row.get<int32_t>("itemid");
		int32_t commandId = row.get<int32_t>("command");
		interaction.increase = row.get<int16_t>("closeness");
		interaction.prob = row.get<uint32_t>("success");

		m_petInteractInfo[itemId][commandId] = interaction;
	}
}

auto ItemDataProvider::getCardId(int32_t mobId) const -> int32_t {
	auto kvp = m_mobsToCards.find(mobId);
	if (kvp == std::end(m_mobsToCards)) {
		std::cerr << "Mob out of range for mob ID " << mobId << std::endl;
		return 0;
	}
	return kvp->second;
}

auto ItemDataProvider::getMobId(int32_t cardId) const -> int32_t {
	auto kvp = m_cardsToMobs.find(cardId);
	if (kvp == std::end(m_cardsToMobs)) {
		std::cerr << "Card out of range for card ID " << cardId << std::endl;
		return 0;
	}
	return kvp->second;
}

auto ItemDataProvider::scrollItem(int32_t scrollId, Item *equip, bool whiteScroll, bool gmScroller, int8_t &succeed, bool &cursed) const -> void {
	if (m_scrollInfo.find(scrollId) == std::end(m_scrollInfo)) {
		return;
	}

	auto &itemInfo = m_scrollInfo.find(scrollId)->second;

	bool scrollTakesSlot = !(itemInfo.preventSlip || itemInfo.warmSupport || itemInfo.recover);
	if (itemInfo.preventSlip || itemInfo.warmSupport) {
		succeed = 0;
		if (gmScroller || Randomizer::rand<uint16_t>(99) < itemInfo.success) {
			if (itemInfo.preventSlip) {
				equip->setPreventSlip(true);
			}
			else {
				equip->setWarmSupport(true);
			}
			succeed = 1;
		}
	}
	else if (itemInfo.randStat) {
		if (equip->getSlots() > 0) {
			succeed = 0;
			if (gmScroller || Randomizer::rand<uint16_t>(99) < itemInfo.success) {
				bool increment = gmScroller || Randomizer::rand<uint8_t>(99) < 50U;
				int16_t variance = Items::StatVariance::Chaos::Normal;
				auto getVariance = [gmScroller, increment, variance]() -> int16_t {
					return gmScroller ? variance : Randomizer::rand<int16_t>(increment ? variance : 0, increment ? 0 : -variance);
				};

				// Gives/takes stats on every stat on the item
				equip->addStr(getVariance(), true);
				equip->addDex(getVariance(), true);
				equip->addInt(getVariance(), true);
				equip->addLuk(getVariance(), true);
				equip->addHp(getVariance(), true);
				equip->addMp(getVariance(), true);
				equip->addWatk(getVariance(), true);
				equip->addMatk(getVariance(), true);
				equip->addWdef(getVariance(), true);
				equip->addMdef(getVariance(), true);
				equip->addAvoid(getVariance(), true);
				equip->addAccuracy(getVariance(), true);
				equip->addHands(getVariance(), true);
				equip->addJump(getVariance(), true);
				equip->addSpeed(getVariance(), true);

				equip->incScrolls();
				equip->decSlots();
				succeed = 1;
			}
		}
	}
	else if (itemInfo.recover > 0) {
		// Apparently global doesn't let you use these scrolls on hammer slots
		//int8_t maxSlots = EquipDataProvider::getInstance().getSlots(equip->getId()) + static_cast<int8_t>(equip->getHammers());
		int8_t maxSlots = EquipDataProvider::getInstance().getSlots(equip->getId());
		int8_t maxRecoverableSlots = maxSlots - equip->getScrolls();
		int8_t recoverSlots = std::min(itemInfo.recover, maxRecoverableSlots);
		if (recoverSlots > 0) {
			succeed = 0;
			if (gmScroller || Randomizer::rand<uint16_t>(99) < itemInfo.success) {
				// Give back slot(s)
				equip->incSlots(recoverSlots);
				succeed = 1;
			}
		}
	}
	else {
		if (GameLogicUtilities::itemTypeToScrollType(equip->getId()) != GameLogicUtilities::getScrollType(scrollId)) {
			// Hacking, equip slot different from the scroll slot
			return;
		}
		if (equip->getSlots() > 0) {
			succeed = 0;
			if (gmScroller || Randomizer::rand<uint16_t>(99) < itemInfo.success) {
				succeed = 1;
				equip->addStr(itemInfo.istr);
				equip->addDex(itemInfo.idex);
				equip->addInt(itemInfo.iint);
				equip->addLuk(itemInfo.iluk);
				equip->addHp(itemInfo.ihp);
				equip->addMp(itemInfo.imp);
				equip->addWatk(itemInfo.iwAtk);
				equip->addMatk(itemInfo.imAtk);
				equip->addWdef(itemInfo.iwDef);
				equip->addMdef(itemInfo.imDef);
				equip->addAccuracy(itemInfo.iacc);
				equip->addAvoid(itemInfo.iavo);
				equip->addHands(itemInfo.ihand);
				equip->addJump(itemInfo.ijump);
				equip->addSpeed(itemInfo.ispeed);
				equip->incScrolls();
				equip->decSlots();
			}
		}
	}

	if (succeed == 0) {
		if (itemInfo.cursed > 0 && Randomizer::rand<uint16_t>(99) < itemInfo.cursed) {
			cursed = true;
		}
		else if (!whiteScroll && scrollTakesSlot) {
			equip->decSlots();
		}
	}
}

auto ItemDataProvider::getItemInfo(int32_t itemId) const -> const ItemInfo * const {
	return ext::find_value_ptr(m_itemInfo, itemId);
}

auto ItemDataProvider::getConsumeInfo(int32_t itemId) const -> const ConsumeInfo * const {
	return ext::find_value_ptr(m_consumeInfo, itemId);
}

auto ItemDataProvider::getPetInfo(int32_t itemId) const -> const PetInfo * const {
	return ext::find_value_ptr(m_petInfo, itemId);
}

auto ItemDataProvider::getInteraction(int32_t itemId, int32_t action) const -> const PetInteractInfo * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_petInteractInfo, itemId), action);
}

auto ItemDataProvider::getItemSkills(int32_t itemId) const -> const vector_t<Skillbook> * const {
	return ext::find_value_ptr(m_skillbooks, itemId);
}

auto ItemDataProvider::getItemRewards(int32_t itemId) const -> const vector_t<ItemRewardInfo> * const {
	return ext::find_value_ptr(m_itemRewards, itemId);
}

auto ItemDataProvider::getItemSummons(int32_t itemId) const -> const vector_t<SummonBag> * const {
	return ext::find_value_ptr(m_summonBags, itemId);
}
