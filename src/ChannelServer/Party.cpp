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
	m_partyId(partyId)
{
}

auto Party::setLeader(int32_t playerId) -> void {
	m_leaderId = playerId;
}

namespace Functors {
	struct JoinPartyUpdate {
		auto operator()(Player *target) -> void {
			PartyPacket::joinParty(target, party, player);
		}
		Party *party;
		string_t player;
	};
}

auto Party::addMember(Player *player, bool first) -> void {
	m_members[player->getId()] = player;
	player->setParty(this);

	if (!first) {
		PlayerData *pData = PlayerDataProvider::getInstance().getPlayerData(player->getId());
		pData->party = getId();

		Functors::JoinPartyUpdate func = {this, player->getName()};
		runFunction(func);
	}
}

auto Party::addMember(int32_t id, bool first) -> void {
	m_members[id] = nullptr;
	if (!first) {
		PlayerData *pData = PlayerDataProvider::getInstance().getPlayerData(id);
		pData->party = getId();

		Functors::JoinPartyUpdate func = {this, pData->name};
		runFunction(func);
	}
}

auto Party::setMember(int32_t playerId, Player *player) -> void {
	m_members[playerId] = player;
}

namespace Functors {
	struct LeavePartyUpdate {
		auto operator()(Player *target) -> void {
			PartyPacket::leaveParty(target, party, playerId, player, kicked);
		}
		Party *party;
		int32_t playerId;
		string_t player;
		bool kicked;
	};
}

auto Party::deleteMember(Player *player, bool kicked) -> void {
	m_members.erase(player->getId());
	player->setParty(nullptr);
	if (Instance *instance = getInstance()) {
		instance->sendMessage(PartyRemoveMember, getId(), player->getId());
	}

	Functors::LeavePartyUpdate func = {this, player->getId(), player->getName(), kicked};
	runFunction(func);
}

auto Party::deleteMember(int32_t id, bool kicked) -> void {
	if (Instance *instance = getInstance()) {
		instance->sendMessage(PartyRemoveMember, getId(), id);
	}
	m_members.erase(id);

	PlayerData *player = PlayerDataProvider::getInstance().getPlayerData(id);
	Functors::LeavePartyUpdate func = {this, id, player->name, kicked};
	runFunction(func);
}

auto Party::disband() -> void {
	if (Instance *instance = getInstance()) {
		instance->sendMessage(PartyDisband, getId());
		setInstance(nullptr);
	}
	auto temp = m_members;
	for (const auto &kvp : temp) {
		if (Player *player = kvp.second) {
			player->setParty(nullptr);
		}
		m_members.erase(kvp.first);
	}
}

auto Party::silentUpdate() -> void {
	runFunction([this](Player *player) {
		PartyPacket::silentUpdate(player, this);
	});
}

auto Party::getMemberByIndex(uint8_t index) -> Player * {
	Player *member = nullptr;
	if (index <= m_members.size()) {
		int8_t f = 0;
		for (const auto &kvp : m_members) {
			f++;
			if (f == index) {
				member = kvp.second;
				break;
			}
		}
	}
	return member;
}

auto Party::runFunction(function_t<void(Player *)> func) -> void {
	for (const auto &kvp : m_members) {
		if (Player *player = kvp.second) {
			func(player);
		}
	}
}

auto Party::getAllPlayerIds() -> vector_t<int32_t> {
	vector_t<int32_t> playerIds;
	for (const auto &kvp : m_members) {
		playerIds.push_back(kvp.first);
	}
	return playerIds;
}

auto Party::getPartyMembers(int32_t mapId) -> vector_t<Player *> {
	vector_t<Player *> players;
	runFunction([&players, &mapId](Player *player) {
		if (mapId == -1 || player->getMapId() == mapId) {
			players.push_back(player);
		}
	});
	return players;
}

auto Party::showHpBar(Player *player) -> void {
	runFunction([&player](Player *testPlayer) {
		if (testPlayer != player && testPlayer->getMapId() == player->getMapId()) {
			PlayerPacket::showHpBar(player, testPlayer);
		}
	});
}

auto Party::receiveHpBar(Player *player) -> void {
	runFunction([&player](Player *testPlayer) {
		if (testPlayer != player && testPlayer->getMapId() == player->getMapId()) {
			PlayerPacket::showHpBar(testPlayer, player);
		}
	});
}

auto Party::getMemberCountOnMap(int32_t mapId) -> int8_t {
	int8_t count = 0;
	for (const auto &kvp : m_members) {
		if (Player *test = kvp.second) {
			if (test->getMapId() == mapId) {
				count++;
			}
		}
	}
	return count;
}

auto Party::isWithinLevelRange(uint8_t lowBound, uint8_t highBound) -> bool {
	bool ret = true;
	for (const auto &kvp : m_members) {
		if (Player *test = kvp.second) {
			if (test->getStats()->getLevel() < lowBound || test->getStats()->getLevel() > highBound) {
				ret = false;
				break;
			}
		}
	}
	return ret;
}

auto Party::warpAllMembers(int32_t mapId, const string_t &portalName) -> void {
	if (Maps::getMap(mapId)) {
		PortalInfo *portal = nullptr;
		if (portalName != "") {
			// Optional portal parameter
			portal = Maps::getMap(mapId)->getPortal(portalName);
		}

		runFunction([&mapId, &portal](Player *test) {
			test->setMap(mapId, portal);
		});
	}
}

auto Party::checkFootholds(int8_t memberCount, const vector_t<vector_t<int16_t>> &footholds) -> bool {
	// Determines if the players are properly arranged (i.e. 5 people on 5 barrels in Kerning PQ)
	bool winner = true;
	int8_t membersOnFootholds = 0;
	hash_set_t<size_t> footholdGroupsUsed;

	for (size_t group = 0; group < footholds.size(); group++) {
		const auto &groupFootholds = footholds[group];
		for (const auto &kvp : m_members) {
			if (Player *test = kvp.second) {
				for (const auto &foothold : groupFootholds) {
					if (test->getFoothold() == foothold) {
						if (footholdGroupsUsed.find(group) != std::end(footholdGroupsUsed)) {
							winner = false;
						}
						else {
							footholdGroupsUsed.insert(group);
							membersOnFootholds++;
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
	if (winner && membersOnFootholds != memberCount) {
		// Not all the foothold groups were indexed
		winner = false;
	}
	return winner;
}

auto Party::verifyFootholds(const vector_t<vector_t<int16_t>> &footholds) -> bool {
	// Determines if the players match your selected footholds
	bool winner = true;
	hash_set_t<size_t> footholdGroupsUsed;

	for (size_t group = 0; group < footholds.size(); group++) {
		const vector_t<int16_t> &groupFootholds = footholds[group];
		for (const auto &kvp : m_members) {
			if (Player *test = kvp.second) {
				for (const auto &foothold : groupFootholds) {
					if (test->getFoothold() == foothold) {
						if (footholdGroupsUsed.find(group) != std::end(footholdGroupsUsed)) {
							winner = false;
						}
						else {
							footholdGroupsUsed.insert(group);
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
		winner = footholdGroupsUsed.size() == footholds.size();
	}
	return winner;
}

auto Party::updatePacket(PacketCreator &packet) -> void {
	size_t offset = Parties::MaxMembers - m_members.size();
	size_t i = 0;
	PlayerData *player;
	int16_t channelId = ChannelServer::getInstance().getChannelId();

	// Add party member IDs to packet
	for (const auto &kvp : m_members) {
		packet.add<int32_t>(kvp.first);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member names to packet
	for (const auto &kvp : m_members) {
		player = PlayerDataProvider::getInstance().getPlayerData(kvp.first);
		packet.addString(player->name, 13);
	}
	for (i = 0; i < offset; i++) {
		packet.addString("", 13);
	}

	// Add party member jobs to packet
	for (const auto &kvp : m_members) {
		player = PlayerDataProvider::getInstance().getPlayerData(kvp.first);
		packet.add<int32_t>(player->job);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member levels to packet
	for (const auto &kvp : m_members) {
		player = PlayerDataProvider::getInstance().getPlayerData(kvp.first);
		packet.add<int32_t>(player->level);
	}
	for (i = 0; i < offset; i++) {
		packet.add<int32_t>(0);
	}

	// Add party member channels to packet
	for (const auto &kvp : m_members) {
		player = PlayerDataProvider::getInstance().getPlayerData(kvp.first);
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
	for (const auto &kvp : m_members) {
		player = PlayerDataProvider::getInstance().getPlayerData(kvp.first);
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

	// Add some portal information (Door?)
	for (const auto &kvp : m_members) {
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