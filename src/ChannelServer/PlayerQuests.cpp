/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "Database.h"
#include "Inventory.h"
#include "Levels.h"
#include "PacketCreator.h"
#include "Player.h"
#include "QuestsPacket.h"
#include "Randomizer.h"
#include "TimeUtilities.h"

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
		if (q->second.mobs.size()) {
			firstrun2 = true;
			for (vector<QuestMob>::iterator v = q->second.mobs.begin(); v != q->second.mobs.end(); v++) {
				if (!firstrun2) {
					query << ",(";
				}
				else {
					firstrun2 = false;
				}
				query << m_player->getId() << ","
					<< q->first << ","
					<< v->id << ","
					<< v->count << ","
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
	QuestMob curmob;
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
			addQuestMobs(previous, true);
			curquest = ActiveQuest();
			curquest.id = current;
			curquest.data = data;
		}
		if (mob != 0) {
			curmob.id = mob;
			curmob.count = res[i]["mobskilled"];
			curquest.mobs.push_back(curmob);
			curmob = QuestMob();
		}
		previous = current;
	}
	if (previous != -1) {
		m_quests[previous] = curquest;
		addQuestMobs(previous, true);
	}

	query << "SELECT questid, endtime FROM completed_quests WHERE charid = " << m_player->getId();
	res = query.store();
	for (size_t i = 0; i < res.size(); i++) {
		m_completed[res[i]["questid"]] = res[i]["endtime"];
	}
}

void PlayerQuests::addQuest(int16_t questid, int32_t npcid) {
	QuestsPacket::acceptQuest(m_player, questid, npcid);
	addQuest(questid);
	QuestRewardsInfo &info = Quests::quests[questid].rewards;
	for (size_t i = 0; i < info.size(); i++) {
		if (!info[i].start) {
			giveRewards(info[i]);
		}
	}
	checkDone(m_quests[questid]);
}

void PlayerQuests::addQuest(int16_t questid) {
	QuestInfo &questinfo = Quests::quests[questid];
	ActiveQuest quest;
	quest.id = questid;
	m_quests[questid] = quest;
	addQuestMobs(questid);
}

void PlayerQuests::addQuestMobs(int16_t questid, bool dbload) {
	QuestInfo &questinfo = Quests::quests[questid];
	if (questinfo.hasRequests(QuestRequestTypes::Mob)) {
		QuestRequest mobs = questinfo.getRequest(QuestRequestTypes::Mob);
		QuestMob mob;
		size_t index = 0;
		for (QuestRequest::iterator i = mobs.begin(); i != mobs.end(); i++) {
			mob = (dbload ? m_quests[questid].mobs[index++] : QuestMob());
			mob.id = i->first;
			mob.maxcount = i->second;
			if (dbload) {
				m_quests[questid].mobs[index - 1] = mob;
			}
			else {
				m_quests[questid].mobs.push_back(mob);
			}
			m_mobtoquest[mob.id].push_back(questid);
		}
	}
}

void PlayerQuests::updateQuestMob(int32_t mobid) {
	if (m_mobtoquest.find(mobid) != m_mobtoquest.end()) {
		int16_t qid = 0;
		ActiveQuest q;
		for (size_t i = 0; i < m_mobtoquest[mobid].size(); i++) {
			qid = m_mobtoquest[mobid][i];
			q = m_quests[qid];
			for (size_t k = 0; k < q.mobs.size(); k++) {
				int16_t maxcount = q.mobs[k].maxcount;
				if (q.mobs[k].id == mobid && !q.done && q.mobs[k].count < maxcount) {
					q.mobs[k].count++;
					QuestsPacket::updateQuest(m_player, q);
					if (q.mobs[k].count == maxcount) {
						checkDone(q);
					}
					m_quests[qid] = q;
					break;
				}
			}
		}
	}
}

void PlayerQuests::checkDone(ActiveQuest &quest) {
	QuestInfo &questinfo = Quests::quests[quest.id];
	QuestRequest reqs;
	quest.done = true;
	if (!questinfo.hasRequests()) {
		return;
	}
	if (questinfo.hasRequests(QuestRequestTypes::Item)) {
		int32_t iid = 0;
		int16_t iamt = 0;
		reqs = questinfo.getRequest(QuestRequestTypes::Item);
		for (QuestRequest::iterator i = reqs.begin(); i != reqs.end(); i++) {
			iid = i->first;
			iamt = i->second;
			if ((m_player->getInventory()->getItemAmount(iid) < iamt && iamt > 0) || (iamt == 0 && m_player->getInventory()->getItemAmount(iid) != 0)) {
				quest.done = false;
				break;
			}
		}
	}
	else if (questinfo.hasRequests(QuestRequestTypes::Mob)) {
		int32_t killed = 0;
		reqs = questinfo.getRequest(QuestRequestTypes::Mob);
		for (QuestRequest::iterator i = reqs.begin(); i != reqs.end(); i++) {
			for (uint32_t j = 0; j < quest.mobs.size(); j++) {
				if (quest.mobs[j].id == i->first) {
					killed = quest.mobs[j].count;
					break;
				}
			}
			if (killed < i->second) {
				quest.done = false;
				break;
			}
		}
	}
	if (quest.done) {
		QuestsPacket::doneQuest(m_player, quest.id);
	}
}

void PlayerQuests::finishQuest(int16_t questid, int32_t npcid) {
	QuestInfo &questinfo = Quests::quests[questid];
	QuestRewardInfo info;
	int32_t chance = 0;
	for (size_t i = 0; i < questinfo.rewards.size(); i++) {
		info = questinfo.rewards[i];
		if (info.start) {
			giveRewards(info);
		}
	}
	if (questinfo.hasRequests(QuestRequestTypes::Mob)) {
		QuestRequest mobs = questinfo.getRequest(QuestRequestTypes::Mob);
		for (QuestRequest::iterator i = mobs.begin(); i != mobs.end(); i++) {
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
	int64_t endtime = TimeUtilities::getKoreanTimestamp();
	m_completed[questid] = endtime;
	QuestsPacket::questFinish(m_player, questid, npcid, questinfo.nextquest, endtime);
}

bool PlayerQuests::giveRewards(const QuestRewardInfo &info) {
	int32_t chance = 0;
	if (info.isexp) {
		Levels::giveExp(m_player, info.id * ChannelServer::Instance()->getQuestExprate(), true);
	}
	else if (info.isitem) {
		if (info.prop == 0) {
			if (info.count > 0) {
				QuestsPacket::giveItem(m_player, info.id, info.count);
				Inventory::addNewItem(m_player, info.id, info.count);
			}
			else if (info.count < 0) {
				QuestsPacket::giveItem(m_player, info.id, info.count);
				Inventory::takeItem(m_player, info.id, -info.count);
			}
			else if (info.id > 0) {
				QuestsPacket::giveItem(m_player, info.id, -m_player->getInventory()->getItemAmount(info.id));
				Inventory::takeItem(m_player, info.id, m_player->getInventory()->getItemAmount(info.id));
			}
		}
		else if (info.prop > 0) {
			chance += info.prop;
		}
	}
	else if (info.ismesos) {
		m_player->getInventory()->modifyMesos(info.id);
		QuestsPacket::giveMesos(m_player, info.id);
	}
	else if (info.isfame) {
		m_player->getStats()->setFame(m_player->getStats()->getBaseStat(Stats::Fame) + static_cast<int16_t>(info.id));
		QuestsPacket::giveFame(m_player, info.id);
	}
	if (chance > 0) {
		int32_t random = Randomizer::Instance()->randInt(chance - 1);
		chance = 0;
		if (info.start && info.isitem && info.prop > 0) {
			if (chance >= random) {
				QuestsPacket::giveItem(m_player, info.id, info.count);
				if (info.count > 0) {
					Inventory::addNewItem(m_player, info.id, info.count);
				}
				else {
					Inventory::takeItem(m_player, info.id, -info.count);
				}
			}
			else {
				chance += info.prop;
			}
		}
	}
	return true;
}

void PlayerQuests::removeQuest(int16_t questid) {
	if (isQuestActive(questid)) {
		m_quests.erase(questid);
		QuestsPacket::forfeitQuest(m_player, questid);
	}
}

bool PlayerQuests::isQuestActive(int16_t questid) {
	return m_quests.find(questid) != m_quests.end();
}

bool PlayerQuests::isQuestComplete(int16_t questid) {
	return m_completed.find(questid) != m_completed.end();
}

void PlayerQuests::connectData(PacketCreator &packet) {
	packet.add<int16_t>(m_quests.size()); // Active quests
	for (map<int16_t, ActiveQuest>::iterator iter = m_quests.begin(); iter != m_quests.end(); iter++) {
		packet.add<int16_t>(iter->first);
		packet.addString(iter->second.getQuestData());
	}

	packet.add<int16_t>(m_completed.size()); // Completed quests
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
		QuestsPacket::updateQuest(m_player, g);
	}
}

string PlayerQuests::getQuestData(int16_t id) {
	return (isQuestActive(id) ? m_quests[id].data : "");
}