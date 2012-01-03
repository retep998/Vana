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
#include "PlayerQuests.h"
#include "ChannelServer.h"
#include "Database.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "PacketCreator.h"
#include "Player.h"
#include "QuestDataProvider.h"
#include "QuestsPacket.h"
#include "Randomizer.h"
#include "TimeUtilities.h"
#include <boost/array.hpp>

PlayerQuests::PlayerQuests(Player *player) :
	m_player(player)
{
	load();
}

void PlayerQuests::save() {
	soci::session &sql = Database::getCharDb();
	int32_t charId = m_player->getId();
	int16_t questId = 0;

	sql.once << "DELETE FROM active_quests WHERE character_id = :char", soci::use(charId, "char");
	sql.once << "DELETE FROM completed_quests WHERE character_id = :char", soci::use(charId, "char");

	if (m_quests.size() > 0) {
		int32_t mobId = 0;
		int16_t killed = 0;
		string data = "";

		soci::statement st = (sql.prepare << "INSERT INTO active_quests (chararcter_id, quest_id, mob_id, quantity_killed, data) " <<
												"VALUES (:char, :quest, :mob, :killed, :data)",
												soci::use(charId, "char"),
												soci::use(questId, "quest"),
												soci::use(mobId, "mob"),
												soci::use(killed, "killed"),
												soci::use(data, "data"));

		for (map<int16_t, ActiveQuest>::iterator q = m_quests.begin(); q != m_quests.end(); ++q) {
			questId = q->first;
			data = q->second.data;

			if (q->second.kills.size()) {
				for (map<int32_t, int16_t, std::less<int32_t>>::iterator v = q->second.kills.begin(); v != q->second.kills.end(); ++v) {
					mobId = v->first;
					killed = v->second;
					st.execute(true);
				}
			}
			else {
				mobId = 0;
				killed = 0;
				st.execute(true);
			}
		}
	}

	if (m_completed.size() > 0) {
		int64_t time = 0;

		soci::statement st = (sql.prepare << "INSERT INTO completed_quests " <<
												"VALUES (:char, :quest, :time)",
												soci::use(charId, "char"),
												soci::use(questId, "quest"),
												soci::use(time, "time"));

		for (map<int16_t, int64_t>::iterator q = m_completed.begin(); q != m_completed.end(); ++q) {
			questId = q->first;
			time = q->second;
			st.execute(true);
		}
	}
}

void PlayerQuests::load() {
	soci::session &sql = Database::getCharDb();
	int32_t charId = m_player->getId();
	int16_t previous = -1;
	int16_t current = 0;
	ActiveQuest curQuest;

	soci::rowset<> rs = (sql.prepare << "SELECT a.quest_id, a.mob_id, a.quantity_killed, a.data FROM active_quests a WHERE a.character_id = :char ORDER BY a.quest_id ASC", soci::use(charId, "char"));

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		current = row.get<int16_t>("quest_id");
		int32_t mob = row.get<int32_t>("mob_id");
		string &data = row.get<string>("data");

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

	for (soci::rowset<>::const_iterator i = rs.begin(); i != rs.end(); ++i) {
		soci::row const &row = *i;

		m_completed[row.get<int16_t>("quest_id")] = row.get<int64_t>("end_time");
	}
}

void PlayerQuests::addQuest(int16_t questId, int32_t npcId) {
	QuestsPacket::acceptQuest(m_player, questId, npcId);

	addQuest(questId);
	giveRewards(questId, true);
	checkDone(m_quests[questId]);
}

void PlayerQuests::addQuest(int16_t questId) {
	ActiveQuest quest;
	quest.id = questId;
	m_quests[questId] = quest;
	addQuestMobs(questId);
}

void PlayerQuests::addQuestMobs(int16_t questId) {
	Quest *questInfo = QuestDataProvider::Instance()->getInfo(questId);
	if (questInfo->hasMobRequests()) {
		auto quest = m_quests;
		auto toQuest = m_mobToQuestMapping;
		questInfo->mobRequestFunc([&questId, &quest, &toQuest](int32_t mobId, int16_t count) -> bool {
			quest[questId].kills[mobId] = 0;
			toQuest[mobId].push_back(questId);
			return false;
		});
	}
}

void PlayerQuests::updateQuestMob(int32_t mobId) {
	if (m_mobToQuestMapping.find(mobId) != m_mobToQuestMapping.end()) {
		int16_t qid = 0;
		ActiveQuest q;
		Quest *realquest;
		for (size_t i = 0; i < m_mobToQuestMapping[mobId].size(); i++) {
			qid = m_mobToQuestMapping[mobId][i];
			q = m_quests[qid];
			realquest = QuestDataProvider::Instance()->getInfo(qid);
			int16_t maxcount = realquest->getMobRequestQuantity(mobId);
			if (!q.done && q.kills[mobId] < maxcount) {
				q.kills[mobId] += 1;
				QuestsPacket::updateQuest(m_player, q);
				if (q.kills[mobId] == maxcount) {
					checkDone(q);
				}
				m_quests[qid] = q;
			}
		}
	}
}

void PlayerQuests::checkDone(ActiveQuest &quest) {
	Quest *questInfo = QuestDataProvider::Instance()->getInfo(quest.id);
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
	else if (questInfo->hasMobRequests()) {
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

void PlayerQuests::finishQuest(int16_t questId, int32_t npcId) {
	Quest *questInfo = QuestDataProvider::Instance()->getInfo(questId);

	if (!giveRewards(questId, false)) {
		// Failed, don't complete the quest yet
		return;
	}

	if (questInfo->hasMobRequests()) {
		auto toQuest = m_mobToQuestMapping;
		questInfo->mobRequestFunc([&questId, &toQuest](int32_t mobId, int16_t count) -> bool {
			for (size_t k = 0; k < toQuest[mobId].size(); k++) {
				if (toQuest[mobId][k] == questId) {
					if (toQuest[mobId].size() == 1) {
						toQuest.erase(mobId);
					}
					else {
						toQuest[mobId].erase(toQuest[mobId].begin() + k);
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

bool PlayerQuests::giveRewards(int16_t questId, bool start) {
	Quest *questInfo = QuestDataProvider::Instance()->getInfo(questId);

	if (!questInfo->hasRewards()) {
		return true;
	}

	int16_t job = m_player->getStats()->getJob();
	boost::array<uint8_t, Inventories::InventoryCount> neededSlots = {0};
	boost::array<bool, Inventories::InventoryCount> chanceItem = {false};
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

	vector<QuestRewardInfo> items;
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
			player->getStats()->giveExp(info.id * ChannelServer::Instance()->getQuestExpRate(), true);
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
		int32_t random = Randomizer::Instance()->randInt(chance - 1);
		chance = 0;
		for (size_t i = 0; i < items.size(); i++) {
			QuestRewardInfo &info = items[i];
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

void PlayerQuests::removeQuest(int16_t questId) {
	if (isQuestActive(questId)) {
		m_quests.erase(questId);
		QuestsPacket::forfeitQuest(m_player, questId);
	}
}

bool PlayerQuests::isQuestActive(int16_t questId) {
	return m_quests.find(questId) != m_quests.end();
}

bool PlayerQuests::isQuestComplete(int16_t questId) {
	return m_completed.find(questId) != m_completed.end();
}

void PlayerQuests::connectData(PacketCreator &packet) {
	packet.add<uint16_t>(m_quests.size());
	for (map<int16_t, ActiveQuest>::iterator iter = m_quests.begin(); iter != m_quests.end(); ++iter) {
		packet.add<int16_t>(iter->first);
		packet.addString(iter->second.getQuestData());
	}

	packet.add<uint16_t>(m_completed.size());
	for (map<int16_t, int64_t>::iterator iter = m_completed.begin(); iter != m_completed.end(); ++iter) {
		packet.add<int16_t>(iter->first);
		packet.add<int64_t>(iter->second);
	}
}

void PlayerQuests::setQuestData(int16_t id, const string &data) {
	if (isQuestActive(id)) {
		ActiveQuest &g = m_quests[id];
		g.data = data;
		m_quests[id] = g;
		QuestsPacket::updateQuest(m_player, g);
	}
}

string PlayerQuests::getQuestData(int16_t id) {
	return (isQuestActive(id) ? m_quests[id].data : "");
}