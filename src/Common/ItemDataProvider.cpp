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

auto ItemDataProvider::loadData(BuffDataProvider &provider) -> void {
	std::cout << std::setw(Initializing::OutputWidth) << std::left << "Initializing Items... ";

	loadItems();
	loadConsumes(provider);
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

	soci::rowset<> rs = (Database::getDataDb().prepare
		<< "SELECT id.*, s.label "
		<< "FROM " << Database::makeDataTable("item_data") << " id "
		<< "LEFT JOIN " << Database::makeDataTable("strings") << " s ON id.itemId = s.objectid AND s.object_type = :item",
		soci::use(string_t("item"), "item"));

	for (const auto &row : rs) {
		ItemInfo item;
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

		item_id_t itemId = row.get<item_id_t>("itemid");
		item.price = row.get<mesos_t>("price");
		item.maxSlot = row.get<slot_qty_t>("max_slot_quantity");
		item.makerLevel = row.get<skill_level_t>("level_for_maker");
		item.maxObtainable = row.get<int32_t>("max_possession_count");
		item.minLevel = row.get<player_level_t>("min_level");
		item.maxLevel = row.get<player_level_t>("max_level");
		item.exp = row.get<experience_t>("experience");
		item.mesos = row.get<mesos_t>("money");
		item.npc = row.get<npc_id_t>("npc");
		item.name = row.get<string_t>("label");

		m_itemInfo[itemId] = item;
	}
}

auto ItemDataProvider::loadScrolls() -> void {
	m_scrollInfo.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("item_scroll_data"));

	for (const auto &row : rs) {
		ScrollInfo item;
		item_id_t itemId = row.get<item_id_t>("itemid");
		item.success = row.get<uint16_t>("success");
		item.cursed = row.get<uint16_t>("break_item");
		item.istr = row.get<stat_t>("istr");
		item.idex = row.get<stat_t>("idex");
		item.iint = row.get<stat_t>("iint");
		item.iluk = row.get<stat_t>("iluk");
		item.ihp = row.get<health_t>("ihp");
		item.imp = row.get<health_t>("imp");
		item.iwAtk = row.get<stat_t>("iwatk");
		item.imAtk = row.get<stat_t>("imatk");
		item.iwDef = row.get<stat_t>("iwdef");
		item.imDef = row.get<stat_t>("imdef");
		item.iacc = row.get<stat_t>("iacc");
		item.iavo = row.get<stat_t>("iavo");
		item.ijump = row.get<stat_t>("ijump");
		item.ispeed = row.get<stat_t>("ispeed");

		StringUtilities::runFlags(row.get<opt_string_t>("flags"), [&item](const string_t &cmp) {
			if (cmp == "rand_stat") item.randStat = true;
			else if (cmp == "recover_slot") item.recover = 1;
			else if (cmp == "warm_support") item.warmSupport = true;
			else if (cmp == "prevent_slip") item.preventSlip = true;
		});

		m_scrollInfo[itemId] = item;
	}
}

auto ItemDataProvider::loadConsumes(BuffDataProvider &provider) -> void {
	m_consumeInfo.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("item_consume_data"));

	for (const auto &row : rs) {
		ConsumeInfo item;
		item_id_t itemId = row.get<item_id_t>("itemid");
		item.effect = row.get<uint8_t>("effect");
		item.hp = row.get<health_t>("hp");
		item.mp = row.get<health_t>("mp");
		item.hpr = row.get<int16_t>("hp_percentage");
		item.mpr = row.get<int16_t>("mp_percentage");
		item.moveTo = row.get<map_id_t>("move_to");
		item.decHunger = row.get<uint8_t>("decrease_hunger");
		item.decFatigue = row.get<uint8_t>("decrease_fatigue");
		item.cp = row.get<uint8_t>("carnival_points");
		item.mcProb = row.get<uint16_t>("prob");
		item.time = row.get<int32_t>("buff_time");
		item.wAtk = row.get<stat_t>("weapon_attack");
		item.mAtk = row.get<stat_t>("magic_attack");
		item.wDef = row.get<stat_t>("weapon_defense");
		item.mDef = row.get<stat_t>("magic_defense");
		item.acc = row.get<stat_t>("accuracy");
		item.avo = row.get<stat_t>("avoid");
		item.speed = row.get<stat_t>("speed");
		item.jump = row.get<stat_t>("jump");

		morph_id_t morphId = row.get<morph_id_t>("morph");
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
			if (cmp == "specific_item") item.dropUpItem = row.get<item_id_t>("drop_up_item");
			else if (cmp == "item_range") item.dropUpItemRange = row.get<int16_t>("drop_up_item_range");
		});

		StringUtilities::runFlags(row.get<opt_string_t>("cure_ailments"), [&item](const string_t &cmp) {
			if (cmp == "darkness") item.ailment |= 0x01;
			else if (cmp == "poison") item.ailment |= 0x02;
			else if (cmp == "curse") item.ailment |= 0x04;
			else if (cmp == "seal") item.ailment |= 0x08;
			else if (cmp == "weakness") item.ailment |= 0x10;
		});

		provider.addItemInfo(itemId, item);
		m_consumeInfo[itemId] = item;
	}
}

auto ItemDataProvider::loadMapRanges() -> void {
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("item_monster_card_map_ranges"));

	for (const auto &row : rs) {
		CardMapRange range;
		item_id_t itemId = row.get<item_id_t>("itemid");
		range.startMap = row.get<map_id_t>("start_map");
		range.endMap = row.get<map_id_t>("end_map");

		m_consumeInfo[itemId].mapRanges.push_back(range);
	}
}

auto ItemDataProvider::loadMultiMorphs() -> void {
	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("item_random_morphs"));

	for (const auto &row : rs) {
		Morph morph;
		item_id_t itemId = row.get<item_id_t>("itemid");
		morph.morph = row.get<morph_id_t>("morphid");
		morph.chance = row.get<int8_t>("success");

		m_consumeInfo[itemId].morphs.push_back(morph);
	}
}

auto ItemDataProvider::loadMonsterCardData() -> void {
	m_cardsToMobs.clear();
	m_mobsToCards.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("monster_card_data"));

	for (const auto &row : rs) {
		item_id_t cardId = row.get<item_id_t>("cardid");
		mob_id_t mobId = row.get<mob_id_t>("mobid");

		m_cardsToMobs.emplace(cardId, mobId);
		m_mobsToCards.emplace(mobId, cardId);
	}
}

auto ItemDataProvider::loadItemSkills() -> void {
	m_skillbooks.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("item_skills"));

	for (const auto &row : rs) {
		Skillbook skill;
		item_id_t itemId = row.get<item_id_t>("itemid");
		skill.skillId = row.get<skill_id_t>("skillid");
		skill.reqLevel = row.get<skill_level_t>("req_skill_level");
		skill.maxLevel = row.get<skill_level_t>("master_level");
		skill.chance = row.get<int8_t>("chance");

		m_skillbooks[itemId].push_back(skill);
	}
}

auto ItemDataProvider::loadSummonBags() -> void {
	m_summonBags.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("item_summons"));

	for (const auto &row : rs) {
		SummonBag summon;
		item_id_t itemId = row.get<item_id_t>("itemid");
		summon.mobId = row.get<mob_id_t>("mobid");
		summon.chance = row.get<uint16_t>("chance");

		m_summonBags[itemId].push_back(summon);
	}
}

auto ItemDataProvider::loadItemRewards() -> void {
	m_itemRewards.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("item_reward_data"));

	for (const auto &row : rs) {
		ItemRewardInfo reward;
		item_id_t itemId = row.get<item_id_t>("itemid");
		reward.rewardId = row.get<item_id_t>("rewardid");
		reward.prob = row.get<uint16_t>("prob");
		reward.quantity = row.get<int16_t>("quantity");
		reward.effect = row.get<string_t>("effect");

		m_itemRewards[itemId].push_back(reward);
	}
}

auto ItemDataProvider::loadPets() -> void {
	m_petInfo.clear();

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("item_pet_data"));

	for (const auto &row : rs) {
		PetInfo pet;
		item_id_t itemId = row.get<item_id_t>("itemid");
		pet.name = row.get<string_t>("default_name");
		pet.hunger = row.get<int32_t>("hunger");
		pet.life = row.get<int32_t>("life");
		pet.limitedLife = row.get<int32_t>("limited_life");
		pet.evolveItem = row.get<item_id_t>("evolution_item");
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

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM " << Database::makeDataTable("item_pet_interactions"));

	for (const auto &row : rs) {
		PetInteractInfo interaction;
		item_id_t itemId = row.get<item_id_t>("itemid");
		int32_t commandId = row.get<int32_t>("command");
		interaction.increase = row.get<int16_t>("closeness");
		interaction.prob = row.get<uint32_t>("success");

		m_petInteractInfo[itemId][commandId] = interaction;
	}
}

auto ItemDataProvider::getCardId(mob_id_t mobId) const -> item_id_t {
	auto kvp = m_mobsToCards.find(mobId);
	if (kvp == std::end(m_mobsToCards)) {
		std::cerr << "Mob out of range for mob ID " << mobId << std::endl;
		return 0;
	}
	return kvp->second;
}

auto ItemDataProvider::getMobId(item_id_t cardId) const -> mob_id_t {
	auto kvp = m_cardsToMobs.find(cardId);
	if (kvp == std::end(m_cardsToMobs)) {
		std::cerr << "Card out of range for card ID " << cardId << std::endl;
		return 0;
	}
	return kvp->second;
}

auto ItemDataProvider::scrollItem(const EquipDataProvider &provider, item_id_t scrollId, Item *equip, bool whiteScroll, bool gmScroller, int8_t &succeed, bool &cursed) const -> HackingResult {
	if (m_scrollInfo.find(scrollId) == std::end(m_scrollInfo)) {
		return HackingResult::DefinitelyHacking;
	}

	auto &itemInfo = m_scrollInfo.find(scrollId)->second;

	bool scrollTakesSlot = !(itemInfo.preventSlip || itemInfo.warmSupport || itemInfo.recover);
	if (scrollTakesSlot && equip->getSlots() == 0) {
		return HackingResult::DefinitelyHacking;
	}

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
				provider.setEquipStats(equip, Items::StatVariance::ChaosNormal, gmScroller, false);

				equip->incScrolls();
				succeed = 1;
			}
		}
	}
	else if (itemInfo.recover > 0) {
		// Apparently global doesn't let you use these scrolls on hammer slots
		//int8_t maxSlots = provider.getSlots(equip->getId()) + static_cast<int8_t>(equip->getHammers());
		int8_t maxSlots = provider.getSlots(equip->getId());
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
			return HackingResult::DefinitelyHacking;
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
			}
		}
	}

	if (succeed == 0) {
		if (itemInfo.cursed > 0 && Randomizer::rand<uint16_t>(99) < itemInfo.cursed) {
			cursed = true;
		}
	}

	if (!whiteScroll && scrollTakesSlot) {
		equip->decSlots();
	}

	return HackingResult::NotHacking;
}

auto ItemDataProvider::getItemInfo(item_id_t itemId) const -> const ItemInfo * const {
	return ext::find_value_ptr(m_itemInfo, itemId);
}

auto ItemDataProvider::getConsumeInfo(item_id_t itemId) const -> const ConsumeInfo * const {
	return ext::find_value_ptr(m_consumeInfo, itemId);
}

auto ItemDataProvider::getPetInfo(item_id_t itemId) const -> const PetInfo * const {
	return ext::find_value_ptr(m_petInfo, itemId);
}

auto ItemDataProvider::getInteraction(item_id_t itemId, int32_t action) const -> const PetInteractInfo * const {
	return ext::find_value_ptr(
		ext::find_value_ptr(m_petInteractInfo, itemId), action);
}

auto ItemDataProvider::getItemSkills(item_id_t itemId) const -> const vector_t<Skillbook> * const {
	return ext::find_value_ptr(m_skillbooks, itemId);
}

auto ItemDataProvider::getItemRewards(item_id_t itemId) const -> const vector_t<ItemRewardInfo> * const {
	return ext::find_value_ptr(m_itemRewards, itemId);
}

auto ItemDataProvider::getItemSummons(item_id_t itemId) const -> const vector_t<SummonBag> * const {
	return ext::find_value_ptr(m_summonBags, itemId);
}
