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
#include "PlayerQuests.hpp"
#include "Common/Algorithm.hpp"
#include "Common/Database.hpp"
#include "Common/GameLogicUtilities.hpp"
#include "Common/QuestDataProvider.hpp"
#include "Common/Randomizer.hpp"
#include "Common/TimeUtilities.hpp"
#include "ChannelServer/ChannelServer.hpp"
#include "ChannelServer/Inventory.hpp"
#include "ChannelServer/Player.hpp"
#include "ChannelServer/QuestsPacket.hpp"
#include <array>

namespace Vana {
namespace ChannelServer {

PlayerQuests::PlayerQuests(Player *player) :
	m_player{player}
{
	load();
}

auto PlayerQuests::save() -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	player_id_t charId = m_player->getId();
	quest_id_t questId = 0;

	sql.once << "DELETE FROM " << db.makeTable("active_quests") << " WHERE character_id = :char", soci::use(charId, "char");
	sql.once << "DELETE FROM " << db.makeTable("completed_quests") << " WHERE character_id = :char", soci::use(charId, "char");

	if (m_quests.size() > 0) {
		mob_id_t mobId = 0;
		uint16_t killed = 0;
		int64_t id = 0;
		opt_string_t data;
		// GCC, as usual, bad with operators
		data = "";

		soci::statement st = (sql.prepare
			<< "INSERT INTO " << db.makeTable("active_quests") << " (character_id, quest_id, data) "
			<< "VALUES (:char, :quest, :data)",
			soci::use(charId, "char"),
			soci::use(questId, "quest"),
			soci::use(data, "data"));

		soci::statement stMobs = (sql.prepare
			<< "INSERT INTO " << db.makeTable("active_quests_mobs") << " (active_quest_id, mob_id, quantity_killed) "
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
				id = db.getLastId<int64_t>();
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
			<< "INSERT INTO " << db.makeTable("completed_quests") << " "
			<< "VALUES (:char, :quest, :time)",
			soci::use(charId, "char"),
			soci::use(questId, "quest"),
			soci::use(time, "time"));

		for (const auto &kvp : m_completed) {
			questId = kvp.first;
			time = kvp.second.getValue();
			st.execute(true);
		}
	}
}

auto PlayerQuests::load() -> void {
	auto &db = Database::getCharDb();
	auto &sql = db.getSession();
	player_id_t charId = m_player->getId();
	quest_id_t previous = 0;
	quest_id_t current = 0;
	bool init = true;
	ActiveQuest curQuest;

	soci::rowset<> rs = (sql.prepare
		<< "SELECT a.quest_id, am.mob_id, am.quantity_killed, a.data "
		<< "FROM " << db.makeTable("active_quests") << " a "
		<< "LEFT OUTER JOIN " << db.makeTable("active_quests_mobs") << " am ON am.active_quest_id = a.id "
		<< "WHERE a.character_id = :char ORDER BY a.quest_id ASC",
		soci::use(charId, "char"));

	for (const auto &row : rs) {
		current = row.get<quest_id_t>("quest_id");
		mob_id_t mob = row.get<mob_id_t>("mob_id");
		string_t data = row.get<string_t>("data");

		if (init) {
			curQuest.id = current;
			curQuest.data = data;
			init = false;
		}
		else if (previous != -1 && current != previous) {
			m_quests[previous] = curQuest;
			curQuest = ActiveQuest{};
			curQuest.id = current;
			curQuest.data = data;
		}
		if (mob != 0) {
			uint16_t kills = row.get<uint16_t>("quantity_killed");
			curQuest.kills[mob] = kills;
			m_mobToQuestMapping[mob].push_back(current);
		}
		previous = current;
	}
	if (!init) {
		m_quests[previous] = curQuest;
	}

	rs = (sql.prepare << "SELECT c.quest_id, c.end_time FROM " << db.makeTable("completed_quests") << " c WHERE c.character_id = :char", soci::use(charId, "char"));

	for (const auto &row : rs) {
		m_completed[row.get<quest_id_t>("quest_id")] = FileTime{row.get<int64_t>("end_time")};
	}
}

auto PlayerQuests::addQuest(quest_id_t questId, npc_id_t npcId) -> void {
	m_player->send(Packets::Quests::acceptQuestNotice(questId));
	m_player->send(Packets::Quests::acceptQuest(questId, npcId));

	ActiveQuest quest;
	quest.id = questId;
	m_quests[questId] = quest;

	auto &questInfo = ChannelServer::getInstance().getQuestDataProvider().getInfo(questId);
	questInfo.forEachRequest(false, [&](const QuestRequestInfo &info) -> IterationResult {
		if (info.isMob) {
			quest.kills[info.id] = 0;
			m_mobToQuestMapping[info.id].push_back(questId);
		}
		return IterationResult::ContinueIterating;
	});

	giveRewards(questId, true);
	checkDone(m_quests[questId]);
}

auto PlayerQuests::updateQuestMob(mob_id_t mobId) -> void {
	auto kvp = m_mobToQuestMapping.find(mobId);
	if (kvp == std::end(m_mobToQuestMapping)) {
		return;
	}

	for (const auto &questId : kvp->second) {
		auto &quest = m_quests[questId];
		if (quest.done) {
			continue;
		}

		auto &questInfo = ChannelServer::getInstance().getQuestDataProvider().getInfo(questId);
		bool possiblyCompleted = false;
		bool anyUpdate = false;
		questInfo.forEachRequest(false, [&](const QuestRequestInfo &info) -> IterationResult {
			if (info.isMob && info.id == mobId && quest.kills[info.id] < info.count) {
				quest.kills[info.id]++;
				anyUpdate = true;
				if (info.count == quest.kills[info.id]) {
					possiblyCompleted = true;
				}
			}
			return IterationResult::ContinueIterating;
		});

		if (anyUpdate) {
			m_player->send(Packets::Quests::updateQuest(quest));
		}
		if (possiblyCompleted) {
			checkDone(quest);
		}
	}
}

auto PlayerQuests::checkDone(ActiveQuest &quest) -> void {
	auto &questInfo = ChannelServer::getInstance().getQuestDataProvider().getInfo(quest.id);

	quest.done = CompletionResult::Complete == questInfo.forEachRequest(false, [&](const QuestRequestInfo &info) -> IterationResult {
		if (info.isMob) {
			if (quest.kills[info.id] < info.count) {
				return IterationResult::StopIterating;
			}
		}
		else if (info.isItem) {
			if (m_player->getInventory()->getItemAmount(info.id) < info.count) {
				return IterationResult::StopIterating;
			}
		}
		return IterationResult::ContinueIterating;
	});

	if (quest.done) {
		m_player->send(Packets::Quests::doneQuest(quest.id));
	}
}

auto PlayerQuests::finishQuest(quest_id_t questId, npc_id_t npcId) -> void {
	auto &questInfo = ChannelServer::getInstance().getQuestDataProvider().getInfo(questId);

	if (giveRewards(questId, false) == Result::Failure) {
		// Don't complete the quest yet
		return;
	}

	questInfo.forEachRequest(false, [&](const QuestRequestInfo &info) -> IterationResult {
		if (info.isMob) {
			auto &mapping = m_mobToQuestMapping[info.id];
			if (mapping.size() == 1) {
				m_mobToQuestMapping.erase(info.id);
			}
			else {
				ext::remove_element(mapping, questId);
			}
		}
		return IterationResult::ContinueIterating;
	});

	m_quests.erase(questId);
	FileTime endTime{};
	m_completed[questId] = endTime;
	m_player->send(Packets::Quests::completeQuestNotice(questId, endTime));
	m_player->send(Packets::Quests::completeQuest(questId, npcId, questInfo.getNextQuest()));
	m_player->sendMap(Packets::Quests::completeQuestAnimation(m_player->getId()));
}

auto PlayerQuests::itemDropAllowed(item_id_t itemId, quest_id_t questId) -> AllowQuestItemResult {
	if (!isQuestActive(questId)) {
		return AllowQuestItemResult::Disallow;
	}
	auto &info = ChannelServer::getInstance().getQuestDataProvider().getInfo(questId);
	slot_qty_t questAmount = 0;
	info.forEachRequest(false, [&questAmount, itemId](const QuestRequestInfo &info) -> IterationResult {
		if (info.isItem && info.id == itemId) {
			questAmount += info.count;
		}
		return IterationResult::ContinueIterating;
	});

	if (m_player->getInventory()->getItemAmount(itemId) >= questAmount) {
		return AllowQuestItemResult::Disallow;
	}

	return AllowQuestItemResult::Allow;
}

auto PlayerQuests::giveRewards(quest_id_t questId, bool start) -> Result {
	auto &questInfo = ChannelServer::getInstance().getQuestDataProvider().getInfo(questId);

	job_id_t job = m_player->getStats()->getJob();
	array_t<inventory_t, Inventories::InventoryCount> neededSlots = {0};
	array_t<bool, Inventories::InventoryCount> chanceItem = {false};

	auto checkRewards = [this, &questId, &neededSlots, &chanceItem](const QuestRewardInfo &info) -> IterationResult {
		if (info.isItem) {
			inventory_t inv = GameLogicUtilities::getInventory(info.id) - 1;
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
			mesos_t mesos = info.id + m_player->getInventory()->getMesos();
			if (mesos < 0) {
				// Will trigger for both too low and too high
				m_player->send(Packets::Quests::questError(questId, Packets::Quests::ErrorNotEnoughMesos));
				return IterationResult::StopIterating;
			}
		}

		return IterationResult::ContinueIterating;
	};

	if (questInfo.forEachReward(start, job, checkRewards) == CompletionResult::Incomplete) {
		return Result::Failure;
	}

	for (inventory_t i = 0; i < Inventories::InventoryCount; i++) {
		if (neededSlots[i] != 0 && m_player->getInventory()->getOpenSlotsNum(i + 1) < neededSlots[i]) {
			m_player->send(Packets::Quests::questError(questId, Packets::Quests::ErrorNoItemSpace));
			return Result::Failure;
		}
	}

	vector_t<QuestRewardInfo> items;
	int32_t chance = 0;
	questInfo.forEachReward(start, job, [this, &chance, &items](const QuestRewardInfo &info) -> IterationResult {
		if (info.isItem && info.prop > 0) {
			chance += info.prop;
			items.push_back(info);
		}
		else if (info.isItem) {
			if (info.count > 0) {
				m_player->send(Packets::Quests::giveItem(info.id, info.count));
				Inventory::addNewItem(m_player, info.id, info.count);
			}
			else if (info.count < 0) {
				m_player->send(Packets::Quests::giveItem(info.id, info.count));
				Inventory::takeItem(m_player, info.id, -info.count);
			}
			else if (info.id > 0) {
				m_player->send(Packets::Quests::giveItem(info.id, -m_player->getInventory()->getItemAmount(info.id)));
				Inventory::takeItem(m_player, info.id, m_player->getInventory()->getItemAmount(info.id));
			}
		}
		else if (info.isExp) {
			m_player->getStats()->giveExp(static_cast<uint32_t>(info.id) * ChannelServer::getInstance().getConfig().rates.questExpRate, true);
		}
		else if (info.isMesos) {
			m_player->getInventory()->modifyMesos(info.id);
			m_player->send(Packets::Quests::giveMesos(info.id));
		}
		else if (info.isFame) {
			m_player->getStats()->setFame(m_player->getStats()->getFame() + static_cast<fame_t>(info.id));
			m_player->send(Packets::Quests::giveFame(info.id));
		}
		else if (info.isBuff) {
			Inventory::useItem(m_player, info.id);
		}
		else if (info.isSkill) {
			m_player->getSkills()->setMaxSkillLevel(info.id, static_cast<skill_level_t>(info.masterLevel), true);
			if (!info.masterLevelOnly && info.count) {
				m_player->getSkills()->addSkillLevel(info.id, static_cast<skill_level_t>(info.count), true);
			}
		}

		return IterationResult::ContinueIterating;
	});

	if (chance > 0) {
		int32_t random = Randomizer::rand<int32_t>(chance - 1);
		chance = 0;
		for (const auto &info : items) {
			if (chance >= random) {
				m_player->send(Packets::Quests::giveItem(info.id, info.count));
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

	return Result::Successful;
}

auto PlayerQuests::removeQuest(quest_id_t questId) -> void {
	if (isQuestActive(questId)) {
		m_quests.erase(questId);
		m_player->send(Packets::Quests::forfeitQuest(questId));
	}
}

auto PlayerQuests::isQuestActive(quest_id_t questId) -> bool {
	return m_quests.find(questId) != std::end(m_quests);
}

auto PlayerQuests::isQuestComplete(quest_id_t questId) -> bool {
	return m_completed.find(questId) != std::end(m_completed);
}

auto PlayerQuests::connectPacket(PacketBuilder &builder) -> void {
	builder.add<uint16_t>(static_cast<uint16_t>(m_quests.size()));
	for (const auto &kvp : m_quests) {
		builder.add<quest_id_t>(kvp.first);
		builder.add<string_t>(kvp.second.getQuestData());
	}

	builder.add<uint16_t>(static_cast<uint16_t>(m_completed.size()));
	for (const auto &kvp : m_completed) {
		builder.add<quest_id_t>(kvp.first);
		builder.add<FileTime>(kvp.second);
	}
}

auto PlayerQuests::setQuestData(quest_id_t id, const string_t &data) -> void {
	// TODO FIXME figure out how this works
	// e.g. Battleship quest
	/*
	if (!isQuestActive(id)) {
		m_quests[id] = ActiveQuest{};
		m_player->send(QuestsPacket::acceptQuest(id, 0));
		m_player->send(QuestsPacket::acceptQuestNotice(id));
	}
	*/

	auto &quest = m_quests[id];
	quest.data = data;
	m_player->send(Packets::Quests::updateQuest(quest));
}

auto PlayerQuests::getQuestData(quest_id_t id) -> string_t {
	return isQuestActive(id) ? m_quests[id].data : "";
}

}
}