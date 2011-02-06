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
#include "PlayerNpcDataProvider.h"
#include "ChannelServer.h"
#include "Database.h"
#include "GameLogicUtilities.h"
#include "InitializeCommon.h"
#include "MapConstants.h"
#include "MapObjects.h"
#include "PacketCreator.h"
#include "Player.h"
#include "SendHeader.h"
#include "SkillConstants.h"
#include "WorldServerConnectPacket.h"

using Initializing::outputWidth;

PlayerNpcDataProvider * PlayerNpcDataProvider::singleton = nullptr;

void PlayerNpcDataProvider::loadData() {
	std::cout << std::setw(outputWidth) << std::left << "Initializing Player NPCs... ";

	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT imitator.* FROM imitate_characters imitator LEFT JOIN characters ch ON ch.ID = imitator.charid WHERE ch.world_id = " << (int16_t)ChannelServer::Instance()->getWorld() << " ORDER BY id ASC";
	mysqlpp::StoreQueryResult res = query.store();

	enum ImitatingCharactersFields {
		Id, CharId, Job, Gender, Skin, Eyes, Hair
	};

	int16_t jobid;
	int32_t tempNpcId;
	for (size_t i = 0; i < res.num_rows(); ++i) {
		mysqlpp::Row &row = res[i];
		jobid = atoi(row[Job]);
		
		tempNpcId = calculateNextNpcId(jobid);
		if (tempNpcId == -1) {
			std::stringstream x;
			x << "Found a Player NPC but all slots for his job are taken already. ID: " << atoi(row[Id]);
			ChannelServer::Instance()->log(LogTypes::Warning, x.str());
		}
		else if (tempNpcId == 0) {
			std::stringstream x;
			x << "Found a Player NPC but his job doesn't have Player NPC's. ID: " << atoi(row[Id]) << ", Job: " << jobid;
			ChannelServer::Instance()->log(LogTypes::Warning, x.str());
		}
		else {
			PlayerNpc *pNpc = new PlayerNpc(tempNpcId,
				atoi(row[CharId]),
				getJobMaxLevel(jobid),
				atoi(row[Gender]),
				atoi(row[Skin]),
				atoi(row[Eyes]),
				atoi(row[Hair]));
			m_player_npcs[tempNpcId] = pNpc;
		}
	}

	std::cout << "DONE" << std::endl;
}

void PlayerNpcDataProvider::loadPlayerNpc(int32_t id) {
	mysqlpp::Query query = Database::getCharDB().query();
	query << "SELECT * FROM imitate_characters WHERE id = " << id;
	mysqlpp::StoreQueryResult res = query.store();

	enum ImitatingCharactersFields {
		Id, CharId, Job, Gender, Skin, Eyes, Hair
	};

	mysqlpp::Row &row = res[0];

	int16_t jobid = atoi(row[Job]);

	int32_t tempNpcId = calculateNextNpcId(jobid);
	PlayerNpc *pNpc = new PlayerNpc(tempNpcId,
		atoi(row[CharId]),
		getJobMaxLevel(jobid),
		atoi(row[Gender]),
		atoi(row[Skin]),
		atoi(row[Eyes]),
		atoi(row[Hair]));
	m_player_npcs[tempNpcId] = pNpc;
}

void PlayerNpcDataProvider::makePacket(vector<NpcSpawnInfo> &npcs, Player *player) {
	vector<int32_t> npcList;
	int32_t tempId;
	for (size_t i = 0; i < npcs.size(); i++) {
		tempId = npcs[i].id;
		if (GameLogicUtilities::isImitatorNpcId(tempId) && m_player_npcs.find(tempId) != m_player_npcs.end()) {
			npcList.push_back(tempId);
		}
	}
	if (npcList.size() == 0) {
		// No Player NPC's
		return;
	}

	PlayerNpc *pNpc;

	// GMS actually sends 1 Player NPC at a time
	PacketCreator packet;
	packet.addHeader(SMSG_PLAYER_NPC_SHOW);
	packet.add<int8_t>(npcList.size());
	for (size_t i = 0; i < npcList.size(); i++) {
		pNpc = m_player_npcs[npcList[i]];
		pNpc->makePacket(packet);
	}
	player->getSession()->send(packet);
}

void PlayerNpcDataProvider::makePlayerNpc(Player *player) {
	for (unordered_map<int32_t, PlayerNpc *>::iterator iter = m_player_npcs.begin(); iter != m_player_npcs.end(); iter++) {
		if (iter->second->getCharacterId() == player->getId()) {
			// We don't want to have a player with 2 or more player NPC's
			return;
		}
	}

	int32_t npcId = calculateNextNpcId(player->getStats()->getJob());

	if (npcId != 0) {
		mysqlpp::Query query = Database::getCharDB().query();

		query << "INSERT INTO imitate_characters (id, charid, job, gender, skin, eyes, hair) VALUES ("
			<< "NULL" << ", "
			<< player->getId() << ", "
			<< player->getStats()->getJob() << ", "
			<< (int16_t)player->getGender() << ", "
			<< (int16_t)player->getSkin() << ", "
			<< player->getEyes() << ", "
			<< player->getHair() << ");";

		query.exec();
		
		int32_t id = (int32_t)query.insert_id();

		query << "REPLACE INTO imitate_items (charid, slot, itemid) "
			<< "SELECT items.charid, items.slot, items.itemid FROM items WHERE charid = " << player->getId() << " AND items.slot < 0";
		query.exec();
		
		WorldServerConnectPacket::loadPlayerNpc(ChannelServer::Instance()->getWorldConnection(), id);
	}
}

int32_t PlayerNpcDataProvider::calculateNextNpcId(int16_t job) {
	int16_t track = GameLogicUtilities::getJobTrack(job);
	int32_t npcId = 0;
	switch (track) {
		case Jobs::JobTracks::Warrior: npcId = getNextNpcId(PlayerNpcs::WarriorRankNpcStart, PlayerNpcs::MaxPlayerNpcsAdventurer); break;
		case Jobs::JobTracks::Magician: npcId = getNextNpcId(PlayerNpcs::MagicianRankNpcStart, PlayerNpcs::MaxPlayerNpcsAdventurer); break;
		case Jobs::JobTracks::Bowman: npcId = getNextNpcId(PlayerNpcs::BowmanRankNpcStart, PlayerNpcs::MaxPlayerNpcsAdventurer); break;
		case Jobs::JobTracks::Thief: npcId = getNextNpcId(PlayerNpcs::ThiefRankNpcStart, PlayerNpcs::MaxPlayerNpcsAdventurer); break;
	}

	if (npcId == 0 && GameLogicUtilities::isCygnusJob(job)) {
		npcId = getNextNpcId(PlayerNpcs::KoCRankNpcStart, PlayerNpcs::MaxPlayerNpcsKoc);
	}
	
	return npcId;
}

int32_t PlayerNpcDataProvider::getNextNpcId(int32_t npcStart, int8_t maxNpcs) {
	for (int8_t i = 0; i <= maxNpcs; i++) {
		if (m_player_npcs.find(npcStart) == m_player_npcs.end()) {
			return npcStart;
		}
		npcStart++;
	}
	return -1;
}

uint8_t PlayerNpcDataProvider::getJobMaxLevel(int16_t jobid) {
	if (GameLogicUtilities::isRegularJob(jobid)) {
		return Stats::PlayerLevels;
	}
	else if (GameLogicUtilities::isCygnusJob(jobid)) {
		return Stats::CygnusLevels;
	}
	return Stats::PlayerLevels; // Default max level, 200
}