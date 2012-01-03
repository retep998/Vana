/*
Copyright (C) 2008-2012 Vana Development Team

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
#include "ItemDataProvider.h"
#include "BuffDataProvider.h"
#include "Database.h"
#include "EquipDataProvider.h"
#include "GameConstants.h"
#include "GameLogicUtilities.h"
#include "GameObjects.h"
#include "InitializeCommon.h"
#include "Randomizer.h"
#include "ShopDataProvider.h"
#include "StringUtilities.h"
#include <iostream>
#include <string>

using boost::bimap;
using std::string;
using Initializing::OutputWidth;
using StringUtilities::runFlags;

ItemDataProvider * ItemDataProvider::singleton = nullptr;

void ItemDataProvider::loadData() {
	std::cout << std::setw(OutputWidth) << std::left << "Initializing Items... ";

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

void ItemDataProvider::loadItems() {
	m_itemInfo.clear();
	int32_t itemId;
	ItemInfo item;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT id.*, s.label " <<
															"FROM item_data id " <<
															"LEFT JOIN strings s ON id.itemId = s.objectid AND s.object_type = :item",
															soci::use(string("item"), "item"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		item = ItemInfo();
		runFlags(row.get<opt_string>("flags"), [&item](const string &cmp) {
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
		item.name = row.get<string>("label");

		m_itemInfo[itemId] = item;
	}
}

void ItemDataProvider::loadScrolls() {
	m_scrollInfo.clear();
	int32_t itemId;
	ScrollInfo item;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_scroll_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		item = ScrollInfo();
		runFlags(row.get<opt_string>("flags"), [&item](const string &cmp) {
			if (cmp == "rand_stat") item.randStat = true;
			else if (cmp == "recover_slot") item.recover = 1;
			else if (cmp == "warm_support") item.warmSupport = true;
			else if (cmp == "prevent_slip") item.preventSlip = true;
		});

		itemId = row.get<int32_t>("itemid");
		item.success = row.get<uint16_t>("success");
		item.cursed = row.get<uint16_t>("break_item");
		item.istr = row.get<int16_t>("istr");
		item.idex = row.get<int16_t>("idex");
		item.iint = row.get<int16_t>("iint");
		item.iluk = row.get<int16_t>("iluk");
		item.ihp = row.get<int16_t>("ihp");
		item.imp = row.get<int16_t>("imp");
		item.iwatk = row.get<int16_t>("iwatk");
		item.imatk = row.get<int16_t>("imatk");
		item.iwdef = row.get<int16_t>("iwdef");
		item.imdef = row.get<int16_t>("imdef");
		item.iacc = row.get<int16_t>("iacc");
		item.iavo = row.get<int16_t>("iavo");
		item.ijump = row.get<int16_t>("ijump");
		item.ispeed = row.get<int16_t>("ispeed");

		m_scrollInfo[itemId] = item;
	}
}

void ItemDataProvider::loadConsumes() {
	m_consumeInfo.clear();
	int32_t itemId;
	int16_t morphId;
	ConsumeInfo item;
	Morph morph;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_consume_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		item = ConsumeInfo();

		runFlags(row.get<opt_string>("flags"), [&item](const string &cmp) {
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

		runFlags(row.get<opt_string>("cure_ailments"), [&item](const string &cmp) {
			if (cmp == "darkness") item.ailment |= 0x01;
			else if (cmp == "poison") item.ailment |= 0x02;
			else if (cmp == "curse") item.ailment |= 0x04;
			else if (cmp == "seal") item.ailment |= 0x08;
			else if (cmp == "weakness") item.ailment |= 0x10;
		});

		itemId = row.get<int32_t>("itemid");
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
		item.watk = row.get<int16_t>("weapon_attack");
		item.matk = row.get<int16_t>("magic_attack");
		item.wdef = row.get<int16_t>("weapon_defense");
		item.mdef = row.get<int16_t>("magic_defense");
		item.acc = row.get<int16_t>("accuracy");
		item.avo = row.get<int16_t>("avoid");
		item.speed = row.get<int16_t>("speed");
		item.jump = row.get<int16_t>("jump");

		morphId = row.get<int16_t>("morph");
		if (morphId) {
			morph = Morph();
			morph.morph = morphId;
			morph.chance = 100;
			item.morphs.push_back(morph);
		}

		runFlags(row.get<opt_string>("drop_up"), [&item, &row](const string &cmp) {
			if (cmp == "none") return;

			item.dropUp = true;
			if (cmp == "specific_item") item.dropUpItem = row.get<int32_t>("drop_up_item");
			else if (cmp == "item_range") item.dropUpItemRange = row.get<int16_t>("drop_up_item_range");
		});

		item.iceResist = row.get<int16_t>("defense_vs_ice");
		item.fireResist = row.get<int16_t>("defense_vs_fire");
		item.lightningResist = row.get<int16_t>("defense_vs_lightning");
		item.poisonResist = row.get<int16_t>("defense_vs_poison");
		item.stunDef = row.get<int16_t>("defense_vs_stun");
		item.darknessDef = row.get<int16_t>("defense_vs_darkness");
		item.weaknessDef = row.get<int16_t>("defense_vs_weakness");
		item.sealDef = row.get<int16_t>("defense_vs_seal");
		item.curseDef = row.get<int16_t>("defense_vs_curse");

		BuffDataProvider::Instance()->addItemInfo(itemId, item);
		m_consumeInfo[itemId] = item;
	}
}

void ItemDataProvider::loadMapRanges() {
	int32_t itemId;
	CardMapRange range;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_monster_card_map_ranges");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		range = CardMapRange();
		itemId = row.get<int32_t>("itemid");
		range.startMap = row.get<int32_t>("start_map");
		range.endMap = row.get<int32_t>("end_map");

		m_consumeInfo[itemId].mapRanges.push_back(range);
	}
}

void ItemDataProvider::loadMultiMorphs() {
	int32_t itemId;
	Morph morph;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_random_morphs");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		morph = Morph();
		itemId = row.get<int32_t>("itemid");
		morph.morph = row.get<int16_t>("morphid");
		morph.chance = row.get<int8_t>("success");

		m_consumeInfo[itemId].morphs.push_back(morph);
	}
}

void ItemDataProvider::loadMonsterCardData() {
	m_cards.clear();
	int32_t cardId;
	int32_t mobId;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM monster_card_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		cardId = row.get<int32_t>("cardid");
		mobId = row.get<int32_t>("mobid");

		m_cards.insert(CardInfo(cardId, mobId));
	}
}

void ItemDataProvider::loadItemSkills() {
	m_skillbooks.clear();
	Skillbook skill;
	int32_t itemId;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_skills");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		skill = Skillbook();
		itemId = row.get<int32_t>("itemid");
		skill.skillId = row.get<int32_t>("skillid");
		skill.reqLevel = row.get<uint8_t>("req_skill_level");
		skill.maxLevel = row.get<uint8_t>("master_level");
		skill.chance = row.get<int8_t>("chance");

		m_skillbooks[itemId].push_back(skill);
	}
}

void ItemDataProvider::loadSummonBags() {
	m_summonBags.clear();
	int32_t itemId;
	SummonBag summon;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_summons");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		itemId = row.get<int32_t>("itemid");
		summon.mobId = row.get<int32_t>("mobid");
		summon.chance = row.get<uint16_t>("chance");

		m_summonBags[itemId].push_back(summon);
	}
}

void ItemDataProvider::loadItemRewards() {
	m_itemRewards.clear();
	int32_t itemId;
	ItemRewardInfo reward;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_reward_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		itemId = row.get<int32_t>("itemid");
		reward.rewardId = row.get<int32_t>("rewardid");
		reward.prob = row.get<uint16_t>("prob");
		reward.quantity = row.get<int16_t>("quantity");
		reward.effect = row.get<string>("effect");

		m_itemRewards[itemId].push_back(reward);
	}
}

void ItemDataProvider::loadPets() {
	m_petInfo.clear();
	PetInfo pet;
	int32_t itemId;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_pet_data");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		pet = PetInfo();
		runFlags(row.get<opt_string>("flags"), [&pet](const string &cmp) {
			if (cmp == "no_revive") pet.noRevive = true;
			else if (cmp == "no_move_to_cash_shop") pet.noStoringInCashShop = true;
			else if (cmp == "auto_react") pet.autoReact = true;
		});

		itemId = row.get<int32_t>("itemid");
		pet.name = row.get<string>("default_name");
		pet.hunger = row.get<int32_t>("hunger");
		pet.life = row.get<int32_t>("life");
		pet.limitedLife = row.get<int32_t>("limited_life");
		pet.evolveItem = row.get<int32_t>("evolution_item");
		pet.evolveLevel = row.get<int8_t>("req_level_for_evolution");

		m_petInfo[itemId] = pet;
	}
}

void ItemDataProvider::loadPetInteractions() {
	m_petInteractInfo.clear();
	PetInteractInfo interact;
	int32_t itemId;
	int32_t commandId;

	soci::rowset<> rs = (Database::getDataDb().prepare << "SELECT * FROM item_pet_interactions");

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		interact = PetInteractInfo();
		itemId = row.get<int32_t>("itemid");
		interact.increase = row.get<int16_t>("closeness");
		interact.prob = row.get<uint32_t>("success");

		m_petInteractInfo[itemId][commandId] = interact;
	}
}

int32_t ItemDataProvider::getCardId(int32_t mobId) {
	try {
		return m_cards.right.at(mobId);
	}
	catch (std::out_of_range) {
		std::cerr << "Mob out of range for mobid " << mobId << std::endl;
	}
	return 0;
}

int32_t ItemDataProvider::getMobId(int32_t cardId) {
	try {
		return m_cards.left.at(cardId);
	}
	catch (std::out_of_range) {
		std::cerr << "Card out of range for cardId " << cardId << std::endl;
	}
	return 0;
}

PetInteractInfo * ItemDataProvider::getInteraction(int32_t itemId, int32_t action) {
	if (m_petInteractInfo.find(itemId) != m_petInteractInfo.end()) {
		if (m_petInteractInfo[itemId].find(action) != m_petInteractInfo[itemId].end()) {
			return &m_petInteractInfo[itemId][action];
		}
	}
	return nullptr;
}

ItemRewardInfo * ItemDataProvider::getRandomReward(int32_t itemId) {
	if (m_itemRewards.find(itemId) == m_itemRewards.end()) {
		return nullptr;
	}
	vector<ItemRewardInfo> *rewards = &m_itemRewards[itemId];
	ItemRewardInfo *info = nullptr;

	for (size_t i = 0; i < rewards->size(); i++) {
		info = &(*rewards)[i];
		if (Randomizer::Instance()->randShort(99) < info->prob) {
			return info;
		}
	}

	return nullptr;
}

void ItemDataProvider::scrollItem(int32_t scrollId, Item *equip, int8_t &succeed, bool &cursed, bool whiteScroll) {
	if (m_scrollInfo.find(scrollId) == m_scrollInfo.end()) {
		return;
	}
	ScrollInfo *itemInfo = &m_scrollInfo[scrollId];

	// Special scrolls
	if (itemInfo->preventSlip || itemInfo->warmSupport) {
		succeed = 0;
		if (Randomizer::Instance()->randShort(99) < itemInfo->success) {
			if (itemInfo->preventSlip) {
				equip->setPreventSlip(true);
			}
			else {
				equip->setWarmSupport(true);
			}
			succeed = 1;
		}
	}
	else {
		// Anything that might have curse on it
		if (itemInfo->randStat) {
			if (equip->getSlots() > 0) {
				succeed = 0;
				if (Randomizer::Instance()->randShort(99) < itemInfo->success) {
					int8_t n = -1;
					uint16_t variance = Items::StatVariance::Chaos::Normal;
					if (Randomizer::Instance()->randShort(99) < 50U) {
						// Increase stats
						n = 1;
					}

					// Gives/takes stats on every stat on the item
					equip->addStr(getStatVariance(n, variance), true);
					equip->addDex(getStatVariance(n, variance), true);
					equip->addInt(getStatVariance(n, variance), true);
					equip->addLuk(getStatVariance(n, variance), true);
					equip->addHp(getStatVariance(n, variance), true);
					equip->addMp(getStatVariance(n, variance), true);
					equip->addWatk(getStatVariance(n, variance), true);
					equip->addMatk(getStatVariance(n, variance), true);
					equip->addWdef(getStatVariance(n, variance), true);
					equip->addMdef(getStatVariance(n, variance), true);
					equip->addAvoid(getStatVariance(n, variance), true);
					equip->addAccuracy(getStatVariance(n, variance), true);
					equip->addHands(getStatVariance(n, variance), true);
					equip->addJump(getStatVariance(n, variance), true);
					equip->addSpeed(getStatVariance(n, variance), true);

					equip->incScrolls();
					equip->decSlots();
					succeed = 1;
				}
			}
		}
		else if (itemInfo->recover > 0) {
			// Apparently global doesn't let you use these scrolls on hammer slots
			//int8_t maxSlots = EquipDataProvider::Instance()->getSlots(equip->getId()) + static_cast<int8_t>(equip->getHammers());
			int8_t maxSlots = EquipDataProvider::Instance()->getSlots(equip->getId());
			int8_t maxRecoverableSlots = maxSlots - equip->getScrolls();
			int8_t recoverSlots = std::min(itemInfo->recover, maxRecoverableSlots);
			if (recoverSlots > 0) {
				succeed = 0;
				if (Randomizer::Instance()->randShort(99) < itemInfo->success) {
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
				if (Randomizer::Instance()->randShort(99) < itemInfo->success) {
					succeed = 1;
					equip->addStr(itemInfo->istr);
					equip->addDex(itemInfo->idex);
					equip->addInt(itemInfo->iint);
					equip->addLuk(itemInfo->iluk);
					equip->addHp(itemInfo->ihp);
					equip->addMp(itemInfo->imp);
					equip->addWatk(itemInfo->iwatk);
					equip->addMatk(itemInfo->imatk);
					equip->addWdef(itemInfo->iwdef);
					equip->addMdef(itemInfo->imdef);
					equip->addAccuracy(itemInfo->iacc);
					equip->addAvoid(itemInfo->iavo);
					equip->addHands(itemInfo->ihand);
					equip->addJump(itemInfo->ijump);
					equip->addSpeed(itemInfo->ispeed);
					equip->incScrolls();
					equip->decSlots();
				}
			}
		}

		if (succeed == 0) {
			if (itemInfo->cursed > 0 && Randomizer::Instance()->randShort(99) < itemInfo->cursed) {
				cursed = true;
			}
			else if (!whiteScroll) {
				equip->decSlots();
			}
		}
	}
}

int16_t ItemDataProvider::getStatVariance(int8_t mod, uint16_t variance) {
	int16_t s = Randomizer::Instance()->randShort(variance);
	s *= mod;
	return s;
}