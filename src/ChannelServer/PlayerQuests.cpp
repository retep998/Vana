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
#include "PlayerQuests.hpp"
#include "ChannelServer.hpp"
#include "Database.hpp"
#include "GameLogicUtilities.hpp"
#include "Inventory.hpp"
#include "PacketCreator.hpp"
#include "Player.hpp"
#include "QuestDataProvider.hpp"
#include "QuestsPacket.hpp"
#include "Randomizer.hpp"
#include "TimeUtilities.hpp"
#include <array>

PlayerQuests::PlayerQuests(Player *player) :
	m_player(player)
{
	load();
}

auto PlayerQuests::save() -> void {
	soci::session &sql = Database::getCharDb();
	int32_t charId = m_player->getId();
	int16_t questId = 0;

	sql.once << "DELETE FROM active_quests WHERE character_id = :char", soci::use(charId, "char");
	sql.once << "DELETE FROM completed_quests WHERE character_id = :char", soci::use(charId, "char");

	if (m_quests.size() > 0) {
		int32_t mobId = 0;
		int16_t killed = 0;
		int64_t id = 0;
		opt_string_t data;
		// GCC, as usual, bad with operators
		data = "";

		soci::statement st = (sql.prepare
			<< "INSERT INTO active_quests (character_id, quest_id, data) "
			<< "VALUES (:char, :quest, :data)",
			soci::use(charId, "char"),
			soci::use(questId, "quest"),
			soci::use(data, "data"));

		soci::statement stMobs = (sql.prepare
			<< "INSERT INTO active_quests_mobs (active_quest_id, mob_id, quantity_killed) "
			<< "VALUES (:id, :mob, :killed)",
			soci::use(id, "id"),
			soci::use(mobId, "mob"),
			soci::use(killed, "killed"));

		for (const auto &kvp : m_quests) {
			const string_t &d = kvp.second.data;
			questId = kvp.first;
			if (d.empty()) {
				data.reset();
			}
			else {
				data = d;
			}
			st.execute(true);

			if (kvp.second.kills.size() > 0) {
				id = Database::getLastId<int64_t>(sql);
				for (const auto &killPair : kvp.second.kills) {
					mobId = killPair.first;
					killed = killPair.second;
					stMobs.execute(true);
				}
			}
		}
	}

	if (m_completed.size() > 0) {
		int64_t time = 0;

		soci::statement st = (sql.prepare
			<< "INSERT INTO completed_quests "
			<< "VALUES (:char, :quest, :time)",
			soci::use(charId, "char"),
			soci::use(questId, "quest"),
			soci::use(time, "time"));

		for (const auto &kvp : m_completed) {
			questId = kvp.first;
			time = kvp.second;
			st.execute(true);
		}
	}
}

auto PlayerQuests::load() -> void {
	soci::session &sql = Database::getCharDb();
	int32_t charId = m_player->getId();
	int16_t previous = -1;
	int16_t current = 0;
	ActiveQuest curQuest;

	soci::rowset<> rs = (sql.prepare
		<< "SELECT a.quest_id, am.mob_id, am.quantity_killed, a.data "
		<< "FROM active_quests a "
		<< "LEFT OUTER JOIN active_quests_mobs am ON am.active_quest_id = a.id "
		<< "WHERE a.character_id = :char ORDER BY a.quest_id ASC",
		soci::use(charId, "char"));

	for (const auto &row : rs) {
		current = row.get<int16_t>("quest_id");
		int32_t mob = row.get<int32_t>("mob_id");
		const string_t &data = row.get<string_t>("data");

		if (previous == -1) {
			curQuest.id = current;
			curQuest.data = data;
		}
		else if (previous != -1 && current != previous) {
			m_quests[previous] = curQuest;
			curQuest = ActiveQuest();
			curQuest.id = current;
			curQuest.data = data;
		}
		if (mob != 0) {
			int16_t kills = row.get<int16_t>("quantity_killed");
			curQuest.kills[mob] = kills;
			m_mobToQuestMapping[mob].push_back(current);
		}
		previous = current;
	}
	if (previous != -1) {
		m_quests[previous] = curQuest;
	}

	rs = (sql.prepare << "SELECT c.quest_id, c.end_time FROM completed_quests c WHERE c.character_id = :char", soci::use(charId, "char"));

	for (const auto &row : rs) {
		m_completed[row.get<int16_t>("quest_id")] = row.get<int64_t>("end_time");
	}
}

auto PlayerQuests::addQuest(int16_t questId, int32_t npcId) -> void {
	QuestsPacket::acceptQuest(m_player, questId, npcId);

	addQuest(questId);
	giveRewards(questId, true);
	checkDone(m_quests[questId]);
}

auto PlayerQuests::addQuest(int16_t questId) -> void {
	ActiveQuest quest;
	quest.id = questId;
	m_quests[questId] = quest;
	addQuestMobs(questId);
}

auto PlayerQuests::addQuestMobs(int16_t questId) -> void {
	Quest *questInfo = QuestDataProvider::getInstance().getInfo(questId);
	if (questInfo->hasMobRequests()) {
		auto &questMapping = m_quests;
		auto &mapping = m_mobToQuestMapping;
		questInfo->mobRequestFunc([&questId, &questMapping, &mapping](int32_t mobId, int16_t count) -> bool {
			questMapping[questId].kills[mobId] = 0;
			mapping[mobId].push_back(questId);
			return false;
		});
	}
}

auto PlayerQuests::updateQuestMob(int32_t mobId) -> void {
	auto kvp = m_mobToQuestMapping.find(mobId);
	if (kvp == std::end(m_mobToQuestMapping)) {
		return;
	}

	for (const auto &questId : kvp->second) {
		ActiveQuest &q = m_quests[questId];
		Quest *realQuest = QuestDataProvider::getInstance().getInfo(questId);
		int16_t maxCount = realQuest->getMobRequestQuantity(mobId);
		if (!q.done && q.kills[mobId] < maxCount) {
			q.kills[mobId] += 1;
			QuestsPacket::updateQuest(m_player, q);
			if (q.kills[mobId] == maxCount) {
				checkDone(q);
			}
			m_quests[questId] = q;
		}
	}
}

auto PlayerQuests::checkDone(ActiveQuest &quest) -> void {
	Quest *questInfo = QuestDataProvider::getInstance().getInfo(quest.id);
	quest.done = true;
	if (!questInfo->hasRequests()) {
		return;
	}
	if (questInfo->hasItemRequests()) {
		auto player = m_player;
		questInfo->itemRequestFunc([&quest, &player](int32_t itemId, int16_t amount) -> bool {
			if ((player->getInventory()->getItemAmount(itemId) < amount && amount > 0) || (amount == 0 && player->getInventory()->getItemAmount(itemId) != 0)) {
				quest.done = false;
				return true;
			}
			return false;
		});
	}
	if (quest.done && questInfo->hasMobRequests()) {
		questInfo->mobRequestFunc([&quest](int32_t mobId, int16_t count) -> bool {
			if (quest.kills[mobId] < count) {
				quest.done = false;
				return true;
			}
			return false;
		});
	}
	if (quest.done) {
		QuestsPacket::doneQuest(m_player, quest.id);
	}
}

auto PlayerQuests::finishQuest(int16_t questId, int32_t npcId) -> void {
	Quest *questInfo = QuestDataProvider::getInstance().getInfo(questId);

	if (!giveRewards(questId, false)) {
		// Failed, don't complete the quest yet
		return;
	}

	if (questInfo->hasMobRequests()) {
		auto toQuest = m_mobToQuestMapping;
		questInfo->mobRequestFunc([&questId, &toQuest](int32_t mobId, int16_t count) -> bool {
			auto &quests = toQuest[mobId];
			for (size_t iter = 0; iter < quests.size(); ++iter) {
				if (quests[iter] == questId) {
					if (quests.size() == 1) {
						toQuest.erase(mobId);
					}
					else {
						quests.erase(std::begin(quests) + iter);
					}
					break;
				}
			}
			return false;
		});
	}
	m_quests.erase(questId);
	int64_t endTime = TimeUtilities::getServerTime();
	m_completed[questId] = endTime;
	QuestsPacket::questFinish(m_player, questId, npcId, questInfo->getNextQuest(), endTime);
}

auto PlayerQuests::giveRewards(int16_t questId, bool start) -> bool {
	Quest *questInfo = QuestDataProvider::getInstance().getInfo(questId);

	if (!questInfo->hasRewards()) {
		return true;
	}

	int16_t job = m_player->getStats()->getJob();
	array_t<uint8_t, Inventories::InventoryCount> neededSlots = {0};
	array_t<bool, Inventories::InventoryCount> chanceItem = {false};
	auto player = m_player;

	auto checkRewards = [&questId, &neededSlots, &chanceItem, &player](const QuestRewardInfo &info) -> bool {
		if (info.isItem) {
			uint8_t inv = GameLogicUtilities::getInventory(info.id) - 1;
			if (info.count > 0) {
				if (info.prop > 0 && !chanceItem[inv]) {
					chanceItem[inv] = true;
					neededSlots[inv]++;
				}
				else if (info.prop == 0) {
					neededSlots[inv]++;
				}
			}
		}
		else if (info.isMesos) {
			int32_t m = info.id + player->getInventory()->getMesos();
			if (m < 0) {
				// Will trigger for both too low and too high
				QuestsPacket::questError(player, questId, QuestsPacket::ErrorNotEnoughMesos);
				return true;
			}
		}
		return false;
	};

	if (!questInfo->rewardsFunc(start, job, checkRewards)) {
		return false;
	}

	for (size_t i = 0; i < Inventories::InventoryCount; i++) {
		if (neededSlots[i] != 0 && m_player->getInventory()->getOpenSlotsNum(i + 1) < neededSlots[i]) {
			QuestsPacket::questError(m_player, questId, QuestsPacket::ErrorNoItemSpace);
			return false;
		}
	}

	vector_t<QuestRewardInfo> items;
	int32_t chance = 0;
	questInfo->rewardsFunc(start, job, [&chance, &items, &player](const QuestRewardInfo &info) -> bool {
		if (info.isItem && info.prop > 0) {
			chance += info.prop;
			items.push_back(info);
		}
		else if (info.isItem) {
			if (info.count > 0) {
				QuestsPacket::giveItem(player, info.id, info.count);
				Inventory::addNewItem(player, info.id, info.count);
			}
			else if (info.count < 0) {
				QuestsPacket::giveItem(player, info.id, info.count);
				Inventory::takeItem(player, info.id, -info.count);
			}
			else if (info.id > 0) {
				QuestsPacket::giveItem(player, info.id, -player->getInventory()->getItemAmount(info.id));
				Inventory::takeItem(player, info.id, player->getInventory()->getItemAmount(info.id));
			}
		}
		else if (info.isExp) {
			player->getStats()->giveExp(info.id * ChannelServer::getInstance().getQuestExpRate(), true);
		}
		else if (info.isMesos) {
			player->getInventory()->modifyMesos(info.id);
			QuestsPacket::giveMesos(player, info.id);
		}
		else if (info.isFame) {
			player->getStats()->setFame(player->getStats()->getFame() + static_cast<int16_t>(info.id));
			QuestsPacket::giveFame(player, info.id);
		}
		else if (info.isBuff) {
			Inventory::useItem(player, info.id);
		}
		else if (info.isSkill) {
			player->getSkills()->setMaxSkillLevel(info.id, static_cast<uint8_t>(info.masterLevel), true);
			if (!info.masterLevelOnly && info.count) {
				player->getSkills()->addSkillLevel(info.id, static_cast<uint8_t>(info.count), true);
			}
		}
		return false;
	});

	if (chance > 0) {
		int32_t random = Randomizer::rand<int32_t>(chance - 1);
		chance = 0;
		for (const auto &info : items) {
			if (chance >= random) {
				QuestsPacket::giveItem(m_player, info.id, info.count);
				if (info.count > 0) {
					Inventory::addNewItem(m_player, info.id, info.count);
				}
				else {
					Inventory::takeItem(m_player, info.id, -info.count);
				}
				break;
			}
			else {
				chance += info.prop;
			}
		}
	}
	return true;
}

auto PlayerQuests::removeQuest(int16_t questId) -> void {
	if (isQuestActive(questId)) {
		m_quests.erase(questId);
		QuestsPacket::forfeitQuest(m_player, questId);
	}
}

auto PlayerQuests::isQuestActive(int16_t questId) -> bool {
	return m_quests.find(questId) != std::end(m_quests);
}

auto PlayerQuests::isQuestComplete(int16_t questId) -> bool {
	return m_completed.find(questId) != std::end(m_completed);
}

auto PlayerQuests::connectData(PacketCreator &packet) -> void {
	packet.add<uint16_t>(m_quests.size());
	for (const auto &kvp : m_quests) {
		packet.add<int16_t>(kvp.first);
		packet.addString(kvp.second.getQuestData());
	}

	packet.add<uint16_t>(m_completed.size());
	for (const auto &kvp : m_completed) {
		packet.add<int16_t>(kvp.first);
		packet.add<int64_t>(kvp.second);
	}
}

auto PlayerQuests::setQuestData(int16_t id, const string_t &data) -> void {
	if (isQuestActive(id)) {
		ActiveQuest &g = m_quests[id];
		g.data = data;
		m_quests[id] = g;
		QuestsPacket::updateQuest(m_player, g);
	}
}

auto PlayerQuests::getQuestData(int16_t id) -> string_t {
	return isQuestActive(id) ? m_quests[id].data : "";
}