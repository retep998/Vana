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
using Initializing::outputWidth;
using StringUtilities::runFlags;

ItemDataProvider * ItemDataProvider::singleton = nullptr;

void ItemDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Items... ";

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
			if (cmp == "time_limited") item->timelimited = true;
			else if (cmp == "cash_item") item->cash = true;
			else if (cmp == "no_trade") item->notrade = true;
			else if (cmp == "no_sale") item->nosale = true;
			else if (cmp == "karma_scissorable") item->karmascissors = true;
			else if (cmp == "expire_on_logout") item->expireonlogout = true;
			else if (cmp == "block_pickup") item->blockpickup = true;
			else if (cmp == "quest") item->quest = true;
		}
		ItemInfo *item;
	};
}

void ItemDataProvider::loadItems() {
	items.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT item_data.*, strings.label FROM item_data LEFT JOIN strings ON item_data.itemid = strings.objectid AND strings.object_type = \'item\'");
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
		item.maxslot = atoi(row[SlotMax]);
		item.makerlevel = atoi(row[MakerLevel]);
		item.maxobtainable = atoi(row[MaxAtOnce]);
		item.minlevel = atoi(row[MinLevel]);
		item.maxlevel = atoi(row[MaxLevel]);
		item.exp = atoi(row[Experience]);
		item.mesos = atoi(row[Mesos]);
		item.npc = atoi(row[Npc]);
		if (row[Name]) {
			item.name = row[Name];
		}
		items[id] = item;
	}
}

namespace Functors {
	struct ScrollFlags {
		void operator() (const string &cmp) {
			if (cmp == "rand_stat") item->randstat = true;
			else if (cmp == "recover_slot") item->recover = true;
			else if (cmp == "warm_support") item->warmsupport = true;
			else if (cmp == "prevent_slip") item->preventslip = true;
		}
		ScrollInfo *item;
	};
}

void ItemDataProvider::loadScrolls() {
	scrolls.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM item_scroll_data");
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

		scrolls[id] = item;
	}
}

namespace Functors {
	struct ConsumeFlags {
		void operator() (const string &cmp) {
			if (cmp == "auto_consume") item->autoconsume = true;
			else if (cmp == "party_item") item->party = true;
			else if (cmp == "meso_up") item->mesoup = true;
			else if (cmp == "ignore_physical_defense") item->ignorewdef = true;
			else if (cmp == "ignore_magical_defense") item->ignoremdef = true;
			else if (cmp == "no_mouse_cancel") item->mousecancel = false;
			else if (cmp == "ignore_continent") item->ignorecontinent = true;
			else if (cmp == "ghost") item->ghost = true;
			else if (cmp == "barrier") item->barrier = true;
			else if (cmp == "prevent_drowning") item->preventdrown = true;
			else if (cmp == "prevent_freezing") item->preventfreeze = true;
			else if (cmp == "override_traction") item->overridetraction = true;
			else if (cmp == "drop_up_for_party") item->partydropup = true;
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
	consumes.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM item_consume_data");
	mysqlpp::UseQueryResult res = query.use();
	int32_t id;
	int16_t morphid;
	ConsumeInfo item;
	AilmentInfo ailment;
	Morph morph;
	string dropup;

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
		item.dechunger = atoi(row[DecHunger]);
		item.decfatigue = atoi(row[DecFatigue]);
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

		morphid = atoi(row[Morph]);
		if (morphid) {
			morph.morph = morphid;
			morph.chance = 100;
			item.morphs.push_back(morph);
		}

		dropup = row[DropUp];
		if (dropup != "none") {
			item.dropup = true;
			if (dropup == "specific_item") {
				item.dropupitem = atoi(row[DropUpItem]);
			}
			else if (dropup == "item_range") {
				item.dropupitemrange = atoi(row[DropUpItemRange]);
			}
		}

		item.mcprob = atoi(row[Prob]);
		item.iceresist = atoi(row[IceDef]);
		item.fireresist = atoi(row[FireDef]);
		item.lightningresist = atoi(row[LightningDef]);
		item.poisonresist = atoi(row[PoisonDef]);
		item.stundef = atoi(row[StunDef]);
		item.darknessdef = atoi(row[DarknessDef]);
		item.weaknessdef = atoi(row[WeaknessDef]);
		item.sealdef = atoi(row[SealDef]);
		item.cursedef = atoi(row[CurseDef]);

		BuffDataProvider::Instance()->addItemInfo(id, item);
		consumes[id] = item;
	}
}

void ItemDataProvider::loadMapRanges() {
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM item_monster_card_map_ranges");
	mysqlpp::UseQueryResult res = query.use();
	int32_t id;
	CardMapRange range;

	enum MapRangeData {
		ItemId = 0,
		StartMap, EndMap
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		id = atoi(row[ItemId]);
		range.startmap = atoi(row[StartMap]);
		range.endmap = atoi(row[EndMap]);
		consumes[id].mapranges.push_back(range);
	}
}

void ItemDataProvider::loadMultiMorphs() {
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM item_random_morphs");
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
		consumes[id].morphs.push_back(morph);
	}
}

void ItemDataProvider::loadMonsterCardData() {
	cards.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM monster_card_data");
	mysqlpp::UseQueryResult res = query.use();
	int32_t cardid;
	int32_t mobid;

	enum CardData {
		CardId = 0,
		MobId
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		cardid = atoi(row[CardId]);
		mobid = atoi(row[MobId]);
		cards.insert(card_info(cardid, mobid));
	}
}

void ItemDataProvider::loadItemSkills() {
	skills.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM item_skills");
	mysqlpp::UseQueryResult res = query.use();
	Skillbook skill;
	int32_t itemid;

	enum SkillData {
		ItemId = 0,
		SkillId, ReqLevel, MasterLevel, Chance
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		itemid = atoi(row[ItemId]);
		skill.skillid = atoi(row[SkillId]);
		skill.reqlevel = atoi(row[ReqLevel]);
		skill.maxlevel = atoi(row[MasterLevel]);
		skill.chance = atoi(row[Chance]);
		skills[itemid].push_back(skill);
	}
}

void ItemDataProvider::loadSummonBags() {
	mobs.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM item_summons");
	mysqlpp::UseQueryResult res = query.use();
	int32_t itemid;
	SummonBag summon;

	enum BagData {
		Id = 0,
		ItemId, MobId, Chance
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		itemid = atoi(row[ItemId]);
		summon.mobid = atoi(row[MobId]);
		summon.chance = atoi(row[Chance]);
		mobs[itemid].push_back(summon);
	}
}

void ItemDataProvider::loadItemRewards() {
	itemRewards.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM item_reward_data");
	mysqlpp::UseQueryResult res = query.use();
	int32_t itemid;
	ItemRewardInfo reward;

	enum RewardData {
		Id = 0,
		ItemId, RewardId, Chance, Quantity, Effect
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		itemid = atoi(row[ItemId]);
		reward.rewardid = atoi(row[RewardId]);
		reward.prob = atoi(row[Chance]);
		reward.quantity = atoi(row[Quantity]);
		reward.effect = row[Effect];
		itemRewards[itemid].push_back(reward);
	}
}

namespace Functors {
	struct PetFlags {
		void operator() (const string &cmp) {
			if (cmp == "no_revive") item->norevive = true;
			else if (cmp == "no_move_to_cash_shop") item->nostoreincashshop = true;
			else if (cmp == "auto_react") item->autoreact = true;
		}
		PetInfo *item;
	};
}

void ItemDataProvider::loadPets() {
	petsInfo.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM item_pet_data");
	mysqlpp::UseQueryResult res = query.use();
	PetInfo pet;
	int32_t itemid;

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

		itemid = atoi(row[ItemId]);
		pet.name = row[Name];
		pet.hunger = atoi(row[Hunger]);
		pet.life = atoi(row[Life]);
		pet.limitedlife = atoi(row[LimitedLife]);
		pet.evoitem = atoi(row[EvoItem]);
		pet.evolevel = atoi(row[EvoLevel]);
		petsInfo[itemid] = pet;
	}
}

void ItemDataProvider::loadPetInteractions() {
	petsInteractInfo.clear();
	mysqlpp::Query query = Database::getDataDB().query("SELECT * FROM item_pet_interactions");
	mysqlpp::UseQueryResult res = query.use();
	PetInteractInfo petinteract;
	int32_t petid;
	int32_t commandid;

	enum PetInteractions {
		ItemId = 0,
		CommandId, Closeness, Success
	};

	while (MYSQL_ROW row = res.fetch_raw_row()) {
		petid = atoi(row[ItemId]);
		commandid = atoi(row[CommandId]);

		petinteract.increase = atoi(row[Closeness]);
		petinteract.prob = atoi(row[Success]);

		petsInteractInfo[petid][commandid] = petinteract;
	}
}

int32_t ItemDataProvider::getCardId(int32_t mobid) {
	try {
		return cards.right.at(mobid);
	}
	catch (std::out_of_range) {
		std::cout << "Mob out of range for mobid " << mobid << std::endl;
	}
	return 0;
}

int32_t ItemDataProvider::getMobId(int32_t cardid) {
	try {
		return cards.left.at(cardid);
	}
	catch (std::out_of_range) {
		std::cout << "Card out of range for cardid " << cardid << std::endl;
	}
	return 0;
}

PetInteractInfo * ItemDataProvider::getInteraction(int32_t itemid, int32_t action) {
	if (petsInteractInfo.find(itemid) != petsInteractInfo.end()) {
		if (petsInteractInfo[itemid].find(action) != petsInteractInfo[itemid].end()) {
			return &petsInteractInfo[itemid][action];
		}
	}
	return nullptr;
}

ItemRewardInfo * ItemDataProvider::getRandomReward(int32_t itemid) {
	if (itemRewards.find(itemid) == itemRewards.end())
		return nullptr;

	vector<ItemRewardInfo> *rewards = &itemRewards[itemid];
	ItemRewardInfo *info = nullptr;

	for (size_t i = 0; i < rewards->size(); i++) {
		info = &(*rewards)[i];
		if (Randomizer::Instance()->randShort(99) < info->prob) {
			return info;
		}
	}

	return nullptr;
}

void ItemDataProvider::scrollItem(int32_t scrollid, Item *equip, int8_t &succeed, bool &cursed, bool wscroll) {
	if (scrolls.find(scrollid) == scrolls.end())
		return;

	ScrollInfo *iteminfo = &scrolls[scrollid];
	if (iteminfo->randstat) {
		if (equip->getSlots() > 0) {
			succeed = 0;
			if (Randomizer::Instance()->randShort(99) < iteminfo->success) {
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
	else if (iteminfo->recover) {
		int8_t maxslots = EquipDataProvider::Instance()->getSlots(equip->getId()) + static_cast<int8_t>(equip->getHammers());
		if ((maxslots - equip->getScrolls()) > equip->getSlots()) {
			if (Randomizer::Instance()->randShort(99) < iteminfo->success) {
				// Give back a slot
				equip->incSlots();
				succeed = 1;
			}
			else {
				if (Randomizer::Instance()->randShort(99) < iteminfo->cursed) {
					cursed = true;
				}
				succeed = 0;
			}
		}
	}
	else if (iteminfo->preventslip) {
		if (Randomizer::Instance()->randShort(99) < iteminfo->success) {
			equip->setPreventSlip(true);
			succeed = 1;
		}
		else {
			succeed = 0;
		}
	}
	else if (iteminfo->warmsupport) {
		if (Randomizer::Instance()->randShort(99) < iteminfo->success) {
			equip->setWarmSupport(true);
			succeed = 1;
		}
		else {
			succeed = 0;
		}
	}
	else {
		if (GameLogicUtilities::itemTypeToScrollType(equip->getId()) != GameLogicUtilities::getScrollType(scrollid)) {
			// Hacking, equip slot different from the scroll slot
			return;
		}
		if (equip->getSlots() > 0) {
			if (Randomizer::Instance()->randShort(99) < iteminfo->success) {
				succeed = 1;
				equip->addStr(iteminfo->istr);
				equip->addDex(iteminfo->idex);
				equip->addInt(iteminfo->iint);
				equip->addLuk(iteminfo->iluk);
				equip->addHp(iteminfo->ihp);
				equip->addMp(iteminfo->imp);
				equip->addWatk(iteminfo->iwatk);
				equip->addMatk(iteminfo->imatk);
				equip->addWdef(iteminfo->iwdef);
				equip->addMdef(iteminfo->imdef);
				equip->addAccuracy(iteminfo->iacc);
				equip->addAvoid(iteminfo->iavo);
				equip->addHands(iteminfo->ihand);
				equip->addJump(iteminfo->ijump);
				equip->addSpeed(iteminfo->ispeed);
				equip->incScrolls();
				equip->decSlots();
			}
			else {
				succeed = 0;
				if (Randomizer::Instance()->randShort(99) < iteminfo->cursed) {
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