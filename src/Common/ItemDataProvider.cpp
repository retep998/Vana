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

namespace Functors {
	struct AllItemFlags {
		void operator() (const string &cmp) {
			if (cmp == "time_limited") item->timeLimited = true;
			else if (cmp == "cash_item") item->cash = true;
			else if (cmp == "no_trade") item->noTrade = true;
			else if (cmp == "no_sale") item->noSale = true;
			else if (cmp == "karma_scissorable") item->karmaScissors = true;
			else if (cmp == "expire_on_logout") item->expireOnLogout = true;
			else if (cmp == "block_pickup") item->blockPickup = true;
			else if (cmp == "quest") item->quest = true;
		}
		ItemInfo *item;
	};
}

void ItemDataProvider::loadItems() {
	m_itemInfo.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT item_data.*, strings.label FROM item_data LEFT JOIN strings ON item_data.itemId = strings.objectid AND strings.object_type = \'item\'");
	mysqlpp::UseQueryResult res = query.use();
	int32_t id;
	ItemInfo item;

	using namespace Functors;

	enum ItemData {
		ItemId = 0,
		Inventory, ItemPrice, SlotMax, MaxAtOnce, MinLevel,
		MaxLevel, Experience, Mesos, StateChange, MakerLevel,
		Npc, Flags, Name
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		item = ItemInfo();
		AllItemFlags whoo = {&item};
		runFlags(row[Flags], whoo);

		id = atoi(row[ItemId]);
		item.price = atoi(row[ItemPrice]);
		item.maxSlot = atoi(row[SlotMax]);
		item.makerLevel = atoi(row[MakerLevel]);
		item.maxObtainable = atoi(row[MaxAtOnce]);
		item.minLevel = atoi(row[MinLevel]);
		item.maxLevel = atoi(row[MaxLevel]);
		item.exp = atoi(row[Experience]);
		item.mesos = atoi(row[Mesos]);
		item.npc = atoi(row[Npc]);
		if (row[Name]) {
			item.name = row[Name];
		}
		m_itemInfo[id] = item;
	}
}

namespace Functors {
	struct ScrollFlags {
		void operator() (const string &cmp) {
			if (cmp == "rand_stat") item->randStat = true;
			else if (cmp == "recover_slot") item->recover = true;
			else if (cmp == "warm_support") item->warmSupport = true;
			else if (cmp == "prevent_slip") item->preventSlip = true;
		}
		ScrollInfo *item;
	};
}

void ItemDataProvider::loadScrolls() {
	m_scrollInfo.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM item_scroll_data");
	mysqlpp::UseQueryResult res = query.use();
	int32_t id;
	ScrollInfo item;

	using namespace Functors;

	enum ScrollData {
		ItemId = 0,
		Success, Cursed, Flags, IncStr, IncDex,
		IncInt, IncLuk, IncHp, IncMp, IncWatk,
		IncMatk, IncWdef, IncMdef, IncAcc, IncAvo,
		IncJump, IncSpeed
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		item = ScrollInfo();
		ScrollFlags whoo = {&item};
		runFlags(row[Flags], whoo);

		id = atoi(row[ItemId]);
		item.success = atoi(row[Success]);
		item.cursed = atoi(row[Cursed]);
		item.istr = atoi(row[IncStr]);
		item.idex = atoi(row[IncDex]);
		item.iint = atoi(row[IncInt]);
		item.iluk = atoi(row[IncLuk]);
		item.ihp = atoi(row[IncHp]);
		item.imp = atoi(row[IncMp]);
		item.iwatk = atoi(row[IncWatk]);
		item.imatk = atoi(row[IncMatk]);
		item.iwdef = atoi(row[IncWdef]);
		item.imdef = atoi(row[IncMdef]);
		item.iacc = atoi(row[IncAcc]);
		item.iavo = atoi(row[IncAvo]);
		item.ijump = atoi(row[IncJump]);
		item.ispeed = atoi(row[IncSpeed]);

		m_scrollInfo[id] = item;
	}
}

namespace Functors {
	struct ConsumeFlags {
		void operator() (const string &cmp) {
			if (cmp == "auto_consume") item->autoConsume = true;
			else if (cmp == "party_item") item->party = true;
			else if (cmp == "meso_up") item->mesoUp = true;
			else if (cmp == "ignore_physical_defense") item->ignoreWdef = true;
			else if (cmp == "ignore_magical_defense") item->ignoreMdef = true;
			else if (cmp == "no_mouse_cancel") item->mouseCancel = false;
			else if (cmp == "ignore_continent") item->ignoreContinent = true;
			else if (cmp == "ghost") item->ghost = true;
			else if (cmp == "barrier") item->barrier = true;
			else if (cmp == "prevent_drowning") item->preventDrown = true;
			else if (cmp == "prevent_freezing") item->preventFreeze = true;
			else if (cmp == "override_traction") item->overrideTraction = true;
			else if (cmp == "drop_up_for_party") item->partyDropUp = true;
		}
		ConsumeInfo *item;
	};

	struct AilmentFlags {
		void operator() (const string &cmp) {
			if (cmp == "darkness") item->darkness = true;
			else if (cmp == "weakness") item->weakness = true;
			else if (cmp == "curse") item->curse = true;
			else if (cmp == "seal") item->seal = true;
			else if (cmp == "poison") item->poison = true;
		}
		AilmentInfo *item;
	};
}

void ItemDataProvider::loadConsumes() {
	m_consumeInfo.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM item_consume_data");
	mysqlpp::UseQueryResult res = query.use();
	int32_t id;
	int16_t morphId;
	ConsumeInfo item;
	AilmentInfo ailment;
	Morph morph;
	string dropUp;

	using namespace Functors;

	enum ConsumeableData {
		ItemId = 0,
		Flags, Ailments, Effect, Hp, Mp,
		HpPercentage, MpPercentage, MoveTo, DecHunger, DecFatigue,
		CarnivalPoints, CreateItem, Prob, Time, Watk,
		Matk, Wdef, Mdef, Acc, Avo,
		Speed, Jump, Morph, DropUp, DropUpItem,
		DropUpItemRange, DropUpMapRanges, IceDef, FireDef, LightningDef,
		PoisonDef, StunDef, WeaknessDef, CurseDef, DarknessDef,
		SealDef
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		item = ConsumeInfo();
		ailment = AilmentInfo();
		ConsumeFlags whoot = {&item};
		runFlags(row[Flags], whoot);

		AilmentFlags whoo = {&ailment};
		runFlags(row[Ailments], whoo);
		if (ailment.darkness) item.ailment |= 0x01;
		if (ailment.poison) item.ailment |= 0x02;
		if (ailment.curse) item.ailment |= 0x04;
		if (ailment.seal) item.ailment |= 0x08;
		if (ailment.weakness) item.ailment |= 0x10;

		id = atoi(row[ItemId]);
		item.hp = atoi(row[Hp]);
		item.mp = atoi(row[Mp]);
		item.hpr = atoi(row[HpPercentage]);
		item.mpr = atoi(row[MpPercentage]);
		item.decHunger = atoi(row[DecHunger]);
		item.decFatigue = atoi(row[DecFatigue]);
		item.cp = atoi(row[CarnivalPoints]);
		item.time = atoi(row[Time]);
		item.watk = atoi(row[Watk]);
		item.effect = atoi(row[Effect]);
		item.matk = atoi(row[Matk]);
		item.wdef = atoi(row[Wdef]);
		item.mdef = atoi(row[Mdef]);
		item.acc = atoi(row[Acc]);
		item.avo = atoi(row[Avo]);
		item.jump = atoi(row[Jump]);
		item.speed = atoi(row[Speed]);
		item.moveTo = atoi(row[MoveTo]);

		morphId = atoi(row[Morph]);
		if (morphId) {
			morph.morph = morphId;
			morph.chance = 100;
			item.morphs.push_back(morph);
		}

		dropUp = row[DropUp];
		if (dropUp != "none") {
			item.dropUp = true;
			if (dropUp == "specific_item") {
				item.dropUpitem = atoi(row[DropUpItem]);
			}
			else if (dropUp == "item_range") {
				item.dropUpItemRange = atoi(row[DropUpItemRange]);
			}
		}

		item.mcProb = atoi(row[Prob]);
		item.iceResist = atoi(row[IceDef]);
		item.fireResist = atoi(row[FireDef]);
		item.lightningResist = atoi(row[LightningDef]);
		item.poisonResist = atoi(row[PoisonDef]);
		item.stunDef = atoi(row[StunDef]);
		item.darknessDef = atoi(row[DarknessDef]);
		item.weaknessDef = atoi(row[WeaknessDef]);
		item.sealDef = atoi(row[SealDef]);
		item.curseDef = atoi(row[CurseDef]);

		BuffDataProvider::Instance()->addItemInfo(id, item);
		m_consumeInfo[id] = item;
	}
}

void ItemDataProvider::loadMapRanges() {
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM item_monster_card_map_ranges");
	mysqlpp::UseQueryResult res = query.use();
	int32_t id;
	CardMapRange range;

	enum MapRangeData {
		ItemId = 0,
		StartMap, EndMap
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		id = atoi(row[ItemId]);
		range.startMap = atoi(row[StartMap]);
		range.endMap = atoi(row[EndMap]);
		m_consumeInfo[id].mapRanges.push_back(range);
	}
}

void ItemDataProvider::loadMultiMorphs() {
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM item_random_morphs");
	mysqlpp::UseQueryResult res = query.use();
	int32_t id;
	Morph morph;

	enum MorphsData {
		Id = 0,
		ItemId, Morph, Success
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		id = atoi(row[ItemId]);
		morph.morph = atoi(row[Morph]);
		morph.chance = atoi(row[Success]);
		m_consumeInfo[id].morphs.push_back(morph);
	}
}

void ItemDataProvider::loadMonsterCardData() {
	m_cards.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM monster_card_data");
	mysqlpp::UseQueryResult res = query.use();
	int32_t cardId;
	int32_t mobId;

	enum CardData {
		CardId = 0,
		MobId
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		cardId = atoi(row[CardId]);
		mobId = atoi(row[MobId]);
		m_cards.insert(CardInfo(cardId, mobId));
	}
}

void ItemDataProvider::loadItemSkills() {
	m_skillbooks.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM item_skills");
	mysqlpp::UseQueryResult res = query.use();
	Skillbook skill;
	int32_t itemId;

	enum SkillData {
		ItemId = 0,
		SkillId, ReqLevel, MasterLevel, Chance
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		itemId = atoi(row[ItemId]);
		skill.skillId = atoi(row[SkillId]);
		skill.reqlevel = atoi(row[ReqLevel]);
		skill.maxLevel = atoi(row[MasterLevel]);
		skill.chance = atoi(row[Chance]);
		m_skillbooks[itemId].push_back(skill);
	}
}

void ItemDataProvider::loadSummonBags() {
	m_summonBags.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM item_summons");
	mysqlpp::UseQueryResult res = query.use();
	int32_t itemId;
	SummonBag summon;

	enum BagData {
		Id = 0,
		ItemId, MobId, Chance
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		itemId = atoi(row[ItemId]);
		summon.mobId = atoi(row[MobId]);
		summon.chance = atoi(row[Chance]);
		m_summonBags[itemId].push_back(summon);
	}
}

void ItemDataProvider::loadItemRewards() {
	m_itemRewards.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM item_reward_data");
	mysqlpp::UseQueryResult res = query.use();
	int32_t itemId;
	ItemRewardInfo reward;

	enum RewardData {
		Id = 0,
		ItemId, RewardId, Chance, Quantity, Effect
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		itemId = atoi(row[ItemId]);
		reward.rewardId = atoi(row[RewardId]);
		reward.prob = atoi(row[Chance]);
		reward.quantity = atoi(row[Quantity]);
		reward.effect = row[Effect];
		m_itemRewards[itemId].push_back(reward);
	}
}

namespace Functors {
	struct PetFlags {
		void operator() (const string &cmp) {
			if (cmp == "no_revive") item->noRevive = true;
			else if (cmp == "no_move_to_cash_shop") item->noStoringInCashShop = true;
			else if (cmp == "auto_react") item->autoReact = true;
		}
		PetInfo *item;
	};
}

void ItemDataProvider::loadPets() {
	m_petInfo.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM item_pet_data");
	mysqlpp::UseQueryResult res = query.use();
	PetInfo pet;
	int32_t itemId;

	using namespace Functors;

	enum PetData {
		ItemId = 0,
		Name, Hunger, Life, LimitedLife, EvoItem,
		EvoLevel, Flags
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		pet = PetInfo();
		PetFlags whoo = {&pet};
		runFlags(row[Flags], whoo);

		itemId = atoi(row[ItemId]);
		pet.name = row[Name];
		pet.hunger = atoi(row[Hunger]);
		pet.life = atoi(row[Life]);
		pet.limitedLife = atoi(row[LimitedLife]);
		pet.evolveItem = atoi(row[EvoItem]);
		pet.evolveLevel = atoi(row[EvoLevel]);
		m_petInfo[itemId] = pet;
	}
}

void ItemDataProvider::loadPetInteractions() {
	m_petInteractInfo.clear();
	mysqlpp::Query query = Database::getDataDb().query("SELECT * FROM item_pet_interactions");
	mysqlpp::UseQueryResult res = query.use();
	PetInteractInfo petinteract;
	int32_t petId;
	int32_t commandId;

	enum PetInteractions {
		ItemId = 0,
		CommandId, Closeness, Success
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		petId = atoi(row[ItemId]);
		commandId = atoi(row[CommandId]);

		petinteract.increase = atoi(row[Closeness]);
		petinteract.prob = atoi(row[Success]);

		m_petInteractInfo[petId][commandId] = petinteract;
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

void ItemDataProvider::scrollItem(int32_t scrollId, Item *equip, int8_t &succeed, bool &cursed, bool wscroll) {
	if (m_scrollInfo.find(scrollId) == m_scrollInfo.end()) {
		return;
	}
	ScrollInfo *itemInfo = &m_scrollInfo[scrollId];
	if (itemInfo->randStat) {
		if (equip->getSlots() > 0) {
			succeed = 0;
			if (Randomizer::Instance()->randShort(99) < itemInfo->success) {
				int8_t n = -1;
				if (Randomizer::Instance()->randShort(99) < 50U) {
					// Increase stats
					n = 1;
				}

				// Gives/takes stats on every stat on the item
				equip->addStr(getStatVariance(n), true);
				equip->addDex(getStatVariance(n), true);
				equip->addInt(getStatVariance(n), true);
				equip->addLuk(getStatVariance(n), true);
				equip->addHp(getStatVariance(n), true);
				equip->addMp(getStatVariance(n), true);
				equip->addWatk(getStatVariance(n), true);
				equip->addMatk(getStatVariance(n), true);
				equip->addWdef(getStatVariance(n), true);
				equip->addMdef(getStatVariance(n), true);
				equip->addAvoid(getStatVariance(n), true);
				equip->addAccuracy(getStatVariance(n), true);
				equip->addHands(getStatVariance(n), true);
				equip->addJump(getStatVariance(n), true);
				equip->addSpeed(getStatVariance(n), true);

				equip->incScrolls();
				equip->decSlots();
				succeed = 1;
			}
			else if (!wscroll) {
				equip->decSlots();
			}
		}
	}
	else if (itemInfo->recover) {
		int8_t maxSlots = EquipDataProvider::Instance()->getSlots(equip->getId()) + static_cast<int8_t>(equip->getHammers());
		if ((maxSlots - equip->getScrolls()) > equip->getSlots()) {
			if (Randomizer::Instance()->randShort(99) < itemInfo->success) {
				// Give back a slot
				equip->incSlots();
				succeed = 1;
			}
			else {
				if (Randomizer::Instance()->randShort(99) < itemInfo->cursed) {
					cursed = true;
				}
				succeed = 0;
			}
		}
	}
	else if (itemInfo->preventSlip) {
		if (Randomizer::Instance()->randShort(99) < itemInfo->success) {
			equip->setPreventSlip(true);
			succeed = 1;
		}
		else {
			succeed = 0;
		}
	}
	else if (itemInfo->warmSupport) {
		if (Randomizer::Instance()->randShort(99) < itemInfo->success) {
			equip->setWarmSupport(true);
			succeed = 1;
		}
		else {
			succeed = 0;
		}
	}
	else {
		if (GameLogicUtilities::itemTypeToScrollType(equip->getId()) != GameLogicUtilities::getScrollType(scrollId)) {
			// Hacking, equip slot different from the scroll slot
			return;
		}
		if (equip->getSlots() > 0) {
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
			else {
				succeed = 0;
				if (Randomizer::Instance()->randShort(99) < itemInfo->cursed) {
					cursed = true;
				}
				else if (!wscroll) {
					equip->decSlots();
				}
			}
		}
	}
}

int16_t ItemDataProvider::getStatVariance(int8_t mod) {
	int16_t s = Randomizer::Instance()->randShort(Items::StatVariance::RandScroll);
	s *= mod;
	return s;
}