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
#include "Party.h"
#include "ChannelServer.h"
#include "GameConstants.h"
#include "Instance.h"
#include "InstanceMessageConstants.h"
#include "Maps.h"
#include "PacketCreator.h"
#include "PartyPacket.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "PlayerObjects.h"
#include "PlayerPacket.h"
#include "WorldServerConnectPacket.h"

Party::Party(int32_t partyId) :
	m_partyId(partyId),
	m_instance(nullptr)
{
}

void Party::setLeader(int32_t playerId) {
	m_leaderId = playerId;
}

namespace Functors {
	struct JoinPartyUpdate {
		void operator()(Player *target) {
			PartyPacket::joinParty(target, party, player);
		}
		Party *party;
		string player;
	};
}

void Party::addMember(Player *player, bool first) {
	m_members[player->getId()] = player;
	player->setParty(this);

	if (!first) {
		PlayerData *pdata = PlayerDataProvider::Instance()->getPlayerData(player->getId());
		pdata->party = getId();

		Functors::JoinPartyUpdate func = {this, player->getName()};
		runFunction(func);
	}
}

void Party::addMember(int32_t id, bool first) {
	m_members[id] = nullptr;
	if (!first) {
		PlayerData *player = PlayerDataProvider::Instance()->getPlayerData(id);
		player->party = getId();

		Functors::JoinPartyUpdate func = {this, player->name};
		runFunction(func);
	}
}

void Party::setMember(int32_t playerId, Player *player) {
	m_members[playerId] = player;
}

namespace Functors {
	struct LeavePartyUpdate {
		void operator()(Player *target) {
			PartyPacket::leaveParty(target, party, playerId, player, kicked);
		}
		Party *party;
		int32_t playerId;
		string player;
		bool kicked;
	};
}

void Party::deleteMember(Player *player, bool kicked) {
	m_members.erase(player->getId());
	player->setParty(nullptr);
	if (Instance *instance = getInstance()) {
		instance->sendMessage(PartyRemoveMember, getId(), player->getId());
	}

	Functors::LeavePartyUpdate func = {this, player->getId(), player->getName(), kicked};
	runFunction(func);
}

void Party::deleteMember(int32_t id, bool kicked) {
	if (Instance *instance = getInstance()) {
		instance->sendMessage(PartyRemoveMember, getId(), id);
	}
	m_members.erase(id);

	PlayerData *player = PlayerDataProvider::Instance()->getPlayerData(id);
	Functors::LeavePartyUpdate func = {this, id, player->name, kicked};
	runFunction(func);
}


void Party::disband() {
	if (Instance *i = getInstance()) {
		i->sendMessage(PartyDisband, getId());
		setInstance(nullptr);
	}
	PlayerMap temp = m_members;
	for (PlayerMap::iterator iter = temp.begin(); iter != temp.end(); iter++) {
		if (iter->second != nullptr) {
			iter->second->setParty(nullptr);
		}
		m_members.erase(iter->first);
	}
}

namespace Functors {
	struct UpdatePacket {
		void operator()(Player *player) {
			PartyPacket::silentUpdate(player, party);
		}
		Party *party;
	};
}

void Party::silentUpdate() {
	Functors::UpdatePacket func = {this};
	runFunction(func);
}

Player * Party::getMemberByIndex(uint8_t index) {
	Player *p = nullptr;
	if (index <= m_members.size()) {
		int8_t f = 0;
		for (PlayerMap::iterator iter = m_members.begin(); iter != m_members.end(); iter++) {
			f++;
			if (f == index) {
				p = iter->second;
				break;
			}
		}
	}
	return p;
}

void Party::runFunction(function<void (Player *)> func) {
	for (PlayerMap::iterator iter = m_members.begin(); iter != m_members.end(); iter++) {
		if (iter->second != nullptr) {
			func(iter->second);
		}
	}
}

vector<int32_t> Party::getAllPlayerIds() {
	vector<int32_t> playerIds;
	for (PlayerMap::iterator iter = m_members.begin(); iter != m_members.end(); iter++) {
		playerIds.push_back(iter->first);
	}
	return playerIds;
}

namespace Functors {
	struct GetPartyMembers {
		void operator()(Player *player) {
			if (mapId == -1 || player->getMap() == mapId) {
				vec->push_back(player);
			}
		}
		vector<Player *> *vec;
		int32_t mapId;
	};
}

vector<Player *> Party::getPartyMembers(int32_t mapId) {
	vector<Player *> players;
	Functors::GetPartyMembers func = {&players, mapId};
	runFunction(func);
	return players;
}

namespace Functors {
	struct ShowHpBar {
		void operator()(Player *player) {
			if (unit != player && unit->getMap() == player->getMap()) {
				PlayerPacket::showHpBar(unit, player);
			}
		}
		Player *unit;
	};
	struct ReceiveHpBar {
		void operator()(Player *player) {
			if (unit != player && unit->getMap() == player->getMap()) {
				PlayerPacket::showHpBar(player, unit);
			}
		}
		Player *unit;
	};
}

void Party::showHpBar(Player *player) {
	Functors::ShowHpBar func = {player};
	runFunction(func);
}

void Party::receiveHpBar(Player *player) {
	Functors::ReceiveHpBar func = {player};
	runFunction(func);
}

int8_t Party::getMemberCountOnMap(int32_t mapId) {
	int8_t count = 0;
	Player *test = nullptr;
	for (PlayerMap::iterator iter = m_members.begin(); iter != m_members.end(); iter++) {
		test = iter->second;
		if (test != nullptr && test->getMap() == mapId) {
			count++;
		}
	}
	return count;
}

bool Party::isWithinLevelRange(uint8_t lowBound, uint8_t highBound) {
	bool ret = true;
	Player *test = nullptr;
	for (PlayerMap::iterator iter = m_members.begin(); iter != m_members.end(); iter++) {
		if (test = iter->second) {
			if (test->getStats()->getLevel() < lowBound || test->getStats()->getLevel() > highBound) {
				ret = false;
				break;
			}
		}
	}
	return ret;
}

namespace Functors {
	struct WarpAllMembers {
		void operator()(Player *test) {
			test->setMap(map, portal);
		}
		int32_t map;
		PortalInfo *portal;
	};
}

void Party::warpAllMembers(int32_t mapId, const string &portalName) {
	if (Maps::getMap(mapId)) {
		PortalInfo *portal = nullptr;
		if (portalName != "") { // Optional portal parameter
			portal = Maps::getMap(mapId)->getPortal(portalName);
		}
		Functors::WarpAllMembers func = {mapId, portal};
		runFunction(func);
	}
}

bool Party::checkFootholds(int8_t membercount, const vector<vector<int16_t>> &footholds) {
	// Determines if the players are properly arranged (i.e. 5 people on 5 barrels in Kerning PQ)
	bool winner = true;
	int8_t membersonfootholds = 0;
	Player *test;
	TakenFootholds fhs; // foothold group ID = key

	for (size_t m = 0; m < footholds.size(); m++) {
		fhs[m] = false;
		for (PlayerMap::iterator iter = m_members.begin(); iter != m_members.end(); iter++) {
			if (test = iter->second) {
				for (size_t k = 0; k < footholds[m].size(); k++) {
					if (test->getFh() == footholds[m][k]) {
						if (fhs[m]) {
							winner = false;
						}
						else {
							fhs[m] = true;
							membersonfootholds++;
						}
						break;
					}
				}
			}
			if (!winner) {
				break;
			}
		}
		if (!winner) {
			break;
		}
	}
	if (winner && (membersonfootholds != membercount)) {
		// Not all the foothold groups were indexed
		winner = false;
	}
	return winner;
}

bool Party::verifyFootholds(const vector<vector<int16_t>> &footholds) {
	// Determines if the players match your selected footholds
	bool winner = true;
	Player *test;
	TakenFootholds fhs; // foothold group ID = key

	for (size_t m = 0; m < footholds.size(); m++) {
		fhs[m] = false;
		for (PlayerMap::iterator iter = m_members.begin(); iter != m_members.end(); iter++) {
			test = iter->second;
			if (test = iter->second) {
				for (size_t k = 0; k < footholds[m].size(); k++) {
					if (test->getFh() == footholds[m][k]) {
						if (fhs[m]) {
							winner = false;
						}
						else {
							fhs[m] = true;
						}
						break;
					}
				}
				if (!winner) {
					break;
				}
			}
		}
		if (!winner) {
			break;
		}
	}
	if (winner) {
		for (TakenFootholds::iterator iter = fhs.begin(); iter != fhs.end(); iter++) {
			if (!iter->second) {
				winner = false;
				break;
			}
		}
	}
	return winner;
}

void Party::updatePacket(PacketCreator &packet) {
	size_t offset = 6 - m_members.size();
	size_t i = 0;
	PlayerMap::iterator iter;
	PlayerData *player;
	int16_t channelId = ChannelServer::Instance()->getChannel();

	// Add party member IDs to packet
	for (iter = m_members.begin(); iter != m_members.end(); iter++) {
		packet.add<int32_t>(iter->first);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}
	
	// Add party member names to packet
	for (iter = m_members.begin(); iter != m_members.end(); iter++) {
		player = PlayerDataProvider::Instance()->getPlayerData(iter->first);
		packet.addString(player->name, 13);
	}
	for (i = 0; i < offset; i++) {
		packet.addString("", 13);
	}

	// Add party member jobs to packet
	for (iter = m_members.begin(); iter != m_members.end(); iter++) {
		player = PlayerDataProvider::Instance()->getPlayerData(iter->first);
		packet.add<int32_t>(player->job);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member levels to packet
	for (iter = m_members.begin(); iter != m_members.end(); iter++) {
		player = PlayerDataProvider::Instance()->getPlayerData(iter->first);
		packet.add<int32_t>(player->level);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member channels to packet
	for (iter = m_members.begin(); iter != m_members.end(); iter++) {
		player = PlayerDataProvider::Instance()->getPlayerData(iter->first);
		if (player->channel != -1) {
			packet.add<int32_t>(player->channel); 
		}
		else {
			packet.add<int32_t>(-2);
		}
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(-2);
	}
	
	packet.add<int32_t>(getLeaderId());

	// Add party member maps to packet
	for (iter = m_members.begin(); iter != m_members.end(); iter++) {
		player = PlayerDataProvider::Instance()->getPlayerData(iter->first);
		if (player->channel == channelId) {
			packet.add<int32_t>(player->map); 
		}
		else {
			packet.add<int32_t>(-2);
		}
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(-2);
	}

	// Add some portal shit
	for (iter = m_members.begin(); iter != m_members.end(); iter++) {
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(-1);
		packet.add<int32_t>(-1);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(Maps::NoMap);
		packet.add<int32_t>(-1);
		packet.add<int32_t>(-1);
	}
}