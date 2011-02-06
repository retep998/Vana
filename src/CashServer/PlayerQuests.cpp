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
#include "PlayerQuests.h"
#include "CashServer.h"
#include "Database.h"
#include "GameLogicUtilities.h"
#include "Inventory.h"
#include "PacketCreator.h"
#include "Player.h"
#include "QuestDataProvider.h"
#include "Randomizer.h"
#include "TimeUtilities.h"
#include <boost/array.hpp>

PlayerQuests::PlayerQuests(Player *player) : m_player(player) {
	load();
}

void PlayerQuests::save() {
	mysqlpp::Query query = Database::getCharDB().query();

	query << "DELETE FROM active_quests WHERE charid = " << m_player->getId();
	query.exec();

	bool firstrun = true;
	bool firstrun2 = true;
	for (map<int16_t, ActiveQuest>::iterator q = m_quests.begin(); q != m_quests.end(); q++) {
		if (firstrun) {
			query << "INSERT INTO active_quests (`charid`, `questid`, `mobid`, `mobskilled`, `data`) VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		if (q->second.kills.size()) {
			firstrun2 = true;
			for (map<int32_t, int16_t, std::less<int32_t> >::iterator v = q->second.kills.begin(); v != q->second.kills.end(); v++) {
				if (!firstrun2) {
					query << ",(";
				}
				else {
					firstrun2 = false;
				}
				query << m_player->getId() << ","
					<< q->first << ","
					<< v->first << ","
					<< v->second << ","
					<< mysqlpp::quote << q->second.data << ")";
			}
		}
		else {
			query << m_player->getId() << ","
				<< q->first << ","
				<< 0 << ","
				<< 0 << ","
				<< mysqlpp::quote << q->second.data << ")";
		}
	}
	if (!firstrun)
		query.exec();

	query << "DELETE FROM completed_quests WHERE charid = " << m_player->getId();
	query.exec();

	firstrun = true;
	for (map<int16_t, int64_t>::iterator q = m_completed.begin(); q != m_completed.end(); q++) {
		if (firstrun) {
			query << "INSERT INTO completed_quests VALUES (";
			firstrun = false;
		}
		else {
			query << ",(";
		}
		query << m_player->getId() << ","
			<< q->first << ","
			<< q->second << ")";
	}
	if (!firstrun)
		query.exec();
}

void PlayerQuests::load() {
	mysqlpp::Query query = Database::getCharDB().query();
	int16_t previous = -1;
	int16_t current = 0;
	ActiveQuest curquest;
	string data;
	query << "SELECT questid, mobid, mobskilled, data FROM active_quests WHERE charid = " << m_player->getId() << " ORDER BY questid ASC";
	mysqlpp::StoreQueryResult res = query.store();
	for (size_t i = 0; i < res.num_rows(); i++) {
		current = res[i]["questid"];
		int32_t mob = res[i]["mobid"];
		res[i]["data"].to_string(data);

		if (previous == -1) {
			curquest.id = current;
			curquest.data = data;
		}
		else if (previous != -1 && current != previous) {
			m_quests[previous] = curquest;
			curquest = ActiveQuest();
			curquest.id = current;
			curquest.data = data;
		}
		if (mob != 0) {
			int16_t kills = res[i]["mobskilled"];
			curquest.kills[mob] = kills;
			m_mobtoquest[mob].push_back(current);
		}
		previous = current;
	}
	if (previous != -1) {
		m_quests[previous] = curquest;
	}

	query << "SELECT questid, endtime FROM completed_quests WHERE charid = " << m_player->getId();
	res = query.store();
	for (size_t i = 0; i < res.size(); i++) {
		m_completed[res[i]["questid"]] = res[i]["endtime"];
	}
}

void PlayerQuests::addQuest(int16_t questid, int32_t npcid) {
	addQuest(questid);
	giveRewards(questid, true);
	checkDone(m_quests[questid]);
}

void PlayerQuests::addQuest(int16_t questid) {
	ActiveQuest quest;
	quest.id = questid;
	m_quests[questid] = quest;
	addQuestMobs(questid);
}

void PlayerQuests::addQuestMobs(int16_t questid) {
	Quest *questinfo = QuestDataProvider::Instance()->getInfo(questid);
	if (questinfo->hasMobRequests()) {
		size_t index = 0;
		for (MobRequests::iterator i = questinfo->getMobBegin(); i != questinfo->getMobEnd(); i++) {
			m_quests[questid].kills[i->first] = 0;
			m_mobtoquest[i->first].push_back(questid);
		}
	}
}

void PlayerQuests::updateQuestMob(int32_t mobid) {
	if (m_mobtoquest.find(mobid) != m_mobtoquest.end()) {
		int16_t qid = 0;
		ActiveQuest q;
		Quest *realquest;
		for (size_t i = 0; i < m_mobtoquest[mobid].size(); i++) {
			qid = m_mobtoquest[mobid][i];
			q = m_quests[qid];
			realquest = QuestDataProvider::Instance()->getInfo(qid);
			int16_t maxcount = realquest->getMobRequestQuantity(mobid);
			if (!q.done && q.kills[mobid] < maxcount) {
				q.kills[mobid] += 1;
				if (q.kills[mobid] == maxcount) {
					checkDone(q);
				}
				m_quests[qid] = q;
			}
		}
	}
}

void PlayerQuests::checkDone(ActiveQuest &quest) {
	Quest *questinfo = QuestDataProvider::Instance()->getInfo(quest.id);
	quest.done = true;
	if (!questinfo->hasRequests()) {
		return;
	}
	if (questinfo->hasItemRequests()) {
		int32_t iid = 0;
		int16_t iamt = 0;
		for (ItemRequests::iterator i = questinfo->getItemBegin(); i != questinfo->getItemEnd(); i++) {
			iid = i->first;
			iamt = i->second;
			if ((m_player->getInventory()->getItemAmount(iid) < iamt && iamt > 0) || (iamt == 0 && m_player->getInventory()->getItemAmount(iid) != 0)) {
				quest.done = false;
				break;
			}
		}
	}
	else if (questinfo->hasMobRequests()) {
		int32_t killed = 0;
		for (MobRequests::iterator i = questinfo->getMobBegin(); i != questinfo->getMobEnd(); i++) {
			if (quest.kills[i->first] < i->second) {
				quest.done = false;
				break;
			}
		}
	}
}

void PlayerQuests::finishQuest(int16_t questid, int32_t npcid) {
	Quest *questinfo = QuestDataProvider::Instance()->getInfo(questid);

	if (!giveRewards(questid, false)) { // Failed, don't complete the quest yet
		return;
	}

	if (questinfo->hasMobRequests()) {
		for (MobRequests::iterator i = questinfo->getMobBegin(); i != questinfo->getMobEnd(); i++) {
			for (size_t k = 0; k < m_mobtoquest[i->first].size(); k++) {
				if (m_mobtoquest[i->first][k] == questid) {
					if (m_mobtoquest[i->first].size() == 1) { // Only one quest for this mob
						m_mobtoquest.erase(i->first);
					}
					else {
						m_mobtoquest[i->first].erase(m_mobtoquest[i->first].begin() + k);
					}
					break;
				}
			}
		}
	}
	m_quests.erase(questid);
	int64_t endtime = TimeUtilities::getServerTime();
	m_completed[questid] = endtime;
}

bool PlayerQuests::giveRewards(int16_t questid, bool start) {
	Quest *questinfo = QuestDataProvider::Instance()->getInfo(questid);

	if (!questinfo->hasRewards()) {
		return true;
	}

	QuestRewardInfo info;
	vector<QuestRewardInfo> items;
	int32_t chance = 0;
	Rewards::iterator startiter;
	Rewards::iterator enditer;
	Rewards::iterator sjobiter;
	Rewards::iterator ejobiter;
	Rewards::iterator iter;
	bool jobrewards = false;
	int16_t job = m_player->getStats()->getJob();

	if (start) {
		startiter = questinfo->getStartRewardsBegin();
		enditer = questinfo->getStartRewardsEnd();
		if (questinfo->hasStartJobRewards(job)) {
			sjobiter = questinfo->getStartJobRewardsBegin(job);
			ejobiter = questinfo->getStartJobRewardsEnd(job);
			jobrewards = true;
		}
	}
	else {
		startiter = questinfo->getEndRewardsBegin();
		enditer = questinfo->getEndRewardsEnd();
		if (questinfo->hasEndJobRewards(job)) {
			sjobiter = questinfo->getEndJobRewardsBegin(job);
			ejobiter = questinfo->getEndJobRewardsEnd(job);
			jobrewards = true;
		}
	}
	if (!checkRewards(questid, startiter, enditer) || (jobrewards && !checkRewards(questid, sjobiter, ejobiter))) {
		return false;
	}
	for (iter = startiter; iter != enditer; iter++) { // Give all applicable rewards
		if (iter->isitem && iter->prop > 0) {
			chance += iter->prop;
			items.push_back(*iter);
		}
		else {
			giveRewards(*iter);
		}
	}
	if (jobrewards) {
		for (iter = sjobiter; iter != ejobiter; iter++) {
			if (iter->isitem && iter->prop > 0) {
				chance += iter->prop;
				items.push_back(*iter);		
			}
			else {
				giveRewards(*iter);
			}
		}
	}
	if (chance > 0) {
		int32_t random = Randomizer::Instance()->randInt(chance - 1);
		chance = 0;
		for (size_t i = 0; i < items.size(); i++) {
			info = items[i];
			if (chance >= random) {
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

bool PlayerQuests::checkRewards(int16_t questid, Rewards::iterator &begin, Rewards::iterator &end) {
	boost::array<uint8_t, Inventories::InventoryCount> neededslots = {0};
	boost::array<bool, Inventories::InventoryCount> chanceitem = {false};

	for (Rewards::iterator iter = begin; iter != end; iter++) { // Loop through rewards, make sure it can be done
		if (iter->isitem) {
			uint8_t inv = GameLogicUtilities::getInventory(iter->id) - 1;
			if (iter->count < 0) continue;

			else if (iter->prop > 0 && !chanceitem[inv]) {
				chanceitem[inv] = true;
				neededslots[inv]++;
			}
			else if (iter->prop == 0) {
				neededslots[inv]++;
			}
		}
		else if (iter->ismesos) {
			int32_t m = iter->id + m_player->getInventory()->getMesos();
			if (m < 0) {
				// Will trigger for both too low and too high
				return false;
			}
		}
	}
	return true;
}

bool PlayerQuests::giveRewards(const QuestRewardInfo &info) {
	return true;
}

void PlayerQuests::removeQuest(int16_t questid) {
	if (isQuestActive(questid)) {
		m_quests.erase(questid);
	}
}

bool PlayerQuests::isQuestActive(int16_t questid) {
	return m_quests.find(questid) != m_quests.end();
}

bool PlayerQuests::isQuestComplete(int16_t questid) {
	return m_completed.find(questid) != m_completed.end();
}

void PlayerQuests::connectData(PacketCreator &packet) {
	packet.add<uint16_t>(m_quests.size()); // Active quests
	for (map<int16_t, ActiveQuest>::iterator iter = m_quests.begin(); iter != m_quests.end(); iter++) {
		packet.add<int16_t>(iter->first);
		packet.addString(iter->second.getQuestData());
	}

	packet.add<uint16_t>(m_completed.size()); // Completed quests
	for (map<int16_t, int64_t>::iterator iter = m_completed.begin(); iter != m_completed.end(); iter++) {
		packet.add<int16_t>(iter->first);
		packet.add<int64_t>(iter->second);
	}
}

void PlayerQuests::setQuestData(int16_t id, const string &data) {
	if (isQuestActive(id)) {
		ActiveQuest g = m_quests[id];
		g.data = data;
		m_quests[id] = g;
	}
}

string PlayerQuests::getQuestData(int16_t id) {
	return (isQuestActive(id) ? m_quests[id].data : "");
}
