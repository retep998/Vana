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
#include "Party.hpp"
#include "ChannelServer.hpp"
#include "GameConstants.hpp"
#include "Instance.hpp"
#include "MapPacket.hpp"
#include "Maps.hpp"
#include "PartyPacket.hpp"
#include "Player.hpp"
#include "PlayerDataProvider.hpp"
#include "PlayerPacket.hpp"
#include "PlayerSkills.hpp"
#include "WorldServerConnectPacket.hpp"

namespace Vana {

Party::Party(party_id_t partyId) :
	m_partyId{partyId}
{
}

auto Party::setLeader(player_id_t playerId, bool showPacket) -> void {
	m_leaderId = playerId;
	if (showPacket) {
		runFunction([this, playerId](Player *player) {
			player->send(Packets::Party::setLeader(this, playerId));
		});
	}
}

namespace Functors {
	struct JoinPartyUpdate {
		auto operator()(Player *target) -> void {
			target->send(Packets::Party::joinParty(target->getMapId(), party, player));
		}
		Party *party;
		string_t player;
	};
}

auto Party::addMember(Player *player, bool first) -> void {
	m_members[player->getId()] = player;
	player->setParty(this);
	showHpBar(player);
	receiveHpBar(player);

	if (!first) {
		// This must be executed first in case the player has an open door already
		// The town position will need to change upon joining
		player->getSkills()->onJoinParty(this, player);

		runFunction([&](Player *partyMember) {
			if (partyMember != player) {
				partyMember->getSkills()->onJoinParty(this, player);
			}
		});

		Functors::JoinPartyUpdate func = {this, player->getName()};
		runFunction(func);
	}
}

auto Party::addMember(player_id_t id, const string_t &name, bool first) -> void {
	m_members[id] = nullptr;
	if (!first) {
		Functors::JoinPartyUpdate func = {this, name};
		runFunction(func);
	}
}

auto Party::setMember(player_id_t playerId, Player *player) -> void {
	m_members[playerId] = player;
}

namespace Functors {
	struct LeavePartyUpdate {
		auto operator()(Player *target) -> void {
			target->send(Packets::Party::leaveParty(target->getMapId(), party, playerId, player, kicked));
		}
		Party *party;
		player_id_t playerId;
		string_t player;
		bool kicked;
	};
}

auto Party::deleteMember(Player *player, bool kicked) -> void {
	player->getSkills()->onLeaveParty(this, player, kicked);

	m_members.erase(player->getId());
	player->setParty(nullptr);
	if (Instance *instance = getInstance()) {
		instance->removePartyMember(getId(), player->getId());
	}

	runFunction([&](Player *partyMember) {
		if (partyMember != player) {
			partyMember->getSkills()->onLeaveParty(this, player, kicked);
		}
	});

	Functors::LeavePartyUpdate func = {this, player->getId(), player->getName(), kicked};
	func(player);
	runFunction(func);
}

auto Party::deleteMember(player_id_t id, const string_t &name, bool kicked) -> void {
	if (Instance *instance = getInstance()) {
		instance->removePartyMember(getId(), id);
	}
	m_members.erase(id);

	Functors::LeavePartyUpdate func = {this, id, name, kicked};
	runFunction(func);
}

auto Party::disband() -> void {
	if (Instance *instance = getInstance()) {
		instance->partyDisband(getId());
		setInstance(nullptr);
	}

	runFunction([&](Player *partyMember) {
		partyMember->getSkills()->onPartyDisband(this);
	});

	auto temp = m_members;
	for (const auto &kvp : temp) {
		if (Player *player = kvp.second) {
			player->setParty(nullptr);
			player->send(Packets::Party::disbandParty(this));
		}
		m_members.erase(kvp.first);
	}
}

auto Party::silentUpdate() -> void {
	runFunction([this](Player *player) {
		player->send(Packets::Party::silentUpdate(player->getMapId(), this));
	});
}

auto Party::getMemberByIndex(uint8_t oneBasedIndex) -> Player * {
	Player *member = nullptr;
	if (oneBasedIndex <= m_members.size()) {
		uint8_t f = 0;
		for (const auto &kvp : m_members) {
			f++;
			if (f == oneBasedIndex) {
				member = kvp.second;
				break;
			}
		}
	}
	return member;
}

auto Party::getZeroBasedIndexByMember(Player *player) -> int8_t {
	int8_t index = 0;
	for (const auto &kvp : m_members) {
		if (kvp.second == player) {
			return index;
		}
		index++;
	}
	return -1;
}

auto Party::runFunction(function_t<void(Player *)> func) -> void {
	for (const auto &kvp : m_members) {
		if (Player *player = kvp.second) {
			func(player);
		}
	}
}

auto Party::getAllPlayerIds() -> vector_t<player_id_t> {
	vector_t<player_id_t> playerIds;
	for (const auto &kvp : m_members) {
		playerIds.push_back(kvp.first);
	}
	return playerIds;
}

auto Party::getPartyMembers(map_id_t mapId) -> vector_t<Player *> {
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
			testPlayer->send(Packets::Player::showHpBar(player->getId(), player->getStats()->getHp(), player->getStats()->getMaxHp()));
		}
	});
}

auto Party::receiveHpBar(Player *player) -> void {
	runFunction([&player](Player *testPlayer) {
		if (testPlayer != player && testPlayer->getMapId() == player->getMapId()) {
			player->send(Packets::Player::showHpBar(testPlayer->getId(), testPlayer->getStats()->getHp(), testPlayer->getStats()->getMaxHp()));
		}
	});
}

auto Party::getMemberCountOnMap(map_id_t mapId) -> int8_t {
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

auto Party::isWithinLevelRange(player_level_t lowBound, player_level_t highBound) -> bool {
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

auto Party::warpAllMembers(map_id_t mapId, const string_t &portalName) -> void {
	if (Map *destination = Maps::getMap(mapId)) {
		const PortalInfo * const destinationPortal = destination->queryPortalName(portalName);
		runFunction([&](Player *test) {
			test->setMap(mapId, destinationPortal);
		});
	}
}

auto Party::checkFootholds(int8_t memberCount, const vector_t<vector_t<foothold_id_t>> &footholds) -> Result {
	// Determines if the players are properly arranged (i.e. 5 people on 5 barrels in Kerning PQ)
	Result winner = Result::Successful;
	int8_t membersOnFootholds = 0;
	hash_set_t<size_t> footholdGroupsUsed;

	for (size_t group = 0; group < footholds.size(); group++) {
		const auto &groupFootholds = footholds[group];
		for (const auto &kvp : m_members) {
			if (Player *test = kvp.second) {
				for (const auto &foothold : groupFootholds) {
					if (test->getFoothold() == foothold) {
						if (footholdGroupsUsed.find(group) != std::end(footholdGroupsUsed)) {
							winner = Result::Failure;
						}
						else {
							footholdGroupsUsed.insert(group);
							membersOnFootholds++;
						}
						break;
					}
				}
			}
			if (winner == Result::Failure) {
				break;
			}
		}
		if (winner == Result::Failure) {
			break;
		}
	}
	if (winner == Result::Successful && membersOnFootholds != memberCount) {
		// Not all the foothold groups were indexed
		winner = Result::Failure;
	}
	return winner;
}

auto Party::verifyFootholds(const vector_t<vector_t<foothold_id_t>> &footholds) -> Result {
	// Determines if the players match your selected footholds
	Result winner = Result::Successful;
	hash_set_t<size_t> footholdGroupsUsed;

	for (size_t group = 0; group < footholds.size(); group++) {
		const auto &groupFootholds = footholds[group];
		for (const auto &kvp : m_members) {
			if (Player *test = kvp.second) {
				for (const auto &foothold : groupFootholds) {
					if (test->getFoothold() == foothold) {
						if (footholdGroupsUsed.find(group) != std::end(footholdGroupsUsed)) {
							winner = Result::Failure;
						}
						else {
							footholdGroupsUsed.insert(group);
						}
						break;
					}
				}
				if (winner == Result::Failure) {
					break;
				}
			}
		}
		if (winner == Result::Failure) {
			break;
		}
	}
	if (winner == Result::Successful) {
		winner = footholdGroupsUsed.size() == footholds.size() ? Result::Successful : Result::Failure;
	}
	return winner;
}

}