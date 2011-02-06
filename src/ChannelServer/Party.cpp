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
#include "Instance.h"
#include "InstanceMessageConstants.h"
#include "MapPacket.h"
#include "Maps.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "WorldServerConnectPacket.h"

void Party::setLeader(int32_t playerid, bool firstload) {
	if (!firstload) {
		oldleader.push_back(leaderid);
	}
	leaderid = playerid;
}

void Party::addMember(Player *player) {
	members[player->getId()] = player;
	player->setParty(this);

	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
		Player *cmp = iter->second;
		if (cmp != 0 && player != nullptr) {
			if (cmp->getDoor() != nullptr) {
				Door *ddoor = cmp->getDoor();
				if (player->getMap() == ddoor->getTownId() || player->getMap() == ddoor->getSourceId()) {
					MapPacket::showDoor(player, ddoor, (player->getMap() == ddoor->getTownId()));
				}
			}
			if (player->getDoor() != nullptr) {
				Door *pdoor = player->getDoor();
				if (cmp->getMap() == pdoor->getTownId() || cmp->getMap() == pdoor->getSourceId()) {
					MapPacket::showDoor(cmp, pdoor, (player->getMap() == pdoor->getTownId()));
				}
			}
		}
	}
}

void Party::addMember(int32_t id) {
	members[id] = nullptr;
}

void Party::deleteMember(Player *player) {
	members.erase(player->getId());
	player->setParty(nullptr);

	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
		Player *cmp = iter->second;
		if (cmp != 0 && player != nullptr) {
			if (cmp->getDoor() != nullptr) {
				Door *ddoor = cmp->getDoor();
				if (player->getMap() == ddoor->getTownId() || player->getMap() == ddoor->getSourceId()) {
					MapPacket::showDoorDisappear(player, ddoor);
				}
			}
			if (player->getDoor() != nullptr) {
				Door *pdoor = player->getDoor();
				if (cmp->getMap() == pdoor->getTownId() || cmp->getMap() == pdoor->getSourceId()) {
					MapPacket::showDoorDisappear(player, pdoor);
				}
			}
		}
	}

	if (Instance *i = getInstance()) {
		i->sendMessage(PartyRemoveMember, getId(), player->getId());
	}
}

void Party::deleteMember(int32_t id) {
	if (Instance *i = getInstance()) {
		i->sendMessage(PartyRemoveMember, getId(), id);
	}
	members.erase(id);
}

void Party::disband() {
	if (Instance *i = getInstance()) {
		i->sendMessage(PartyDisband, getId());
		setInstance(nullptr);
	}
	map<int32_t, Player *, std::greater<int32_t> > temp = members;
	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = temp.begin(); iter != temp.end(); iter++) {
		if (iter->second != nullptr) {
			iter->second->setParty(nullptr);
			members.erase(iter->first);
		}
	}
}

Player * Party::getMemberByIndex(uint8_t index) {
	Player *p = nullptr;
	if (index <= members.size()) {
		int8_t f = 0;
		for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
			f++;
			if (f == index) {
				p = iter->second;
				break;
			}
		}
	}
	return p;
}

vector<int32_t> Party::getAllPlayerIds() {
	vector<int32_t> playerids;
	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
		playerids.push_back(iter->first);
	}
	return playerids;
}

vector<Player *> Party::getPartyMembers(int32_t mapid) {
	vector<Player *> players;
	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
		if (iter->second != nullptr) {
			if (mapid == -1 || iter->second->getMap() == mapid) {
				players.push_back(iter->second);
			}
		}
	}
	return players;
}

void Party::setMember(int32_t playerid, Player *player) {
	members[playerid] = player;
}

void Party::showHpBar(Player *player) {
	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
		Player *m_player = iter->second;
		if (m_player != nullptr && m_player != player && m_player->getMap() == player->getMap())
			PlayerPacket::showHpBar(player, m_player);
	}
}

void Party::receiveHpBar(Player *player) {
	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
		Player *m_player = iter->second;
		if (m_player != nullptr && m_player != player && m_player->getMap() == player->getMap())
			PlayerPacket::showHpBar(m_player, player);
	}
}

int8_t Party::getMemberCountOnMap(int32_t mapid) {
	int8_t count = 0;
	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
		Player *m_player = iter->second;
		if (m_player != nullptr && m_player->getMap() == mapid)
			count++;
	}
	return count;
}

bool Party::isWithinLevelRange(uint8_t lowbound, uint8_t highbound) {
	bool is = true;
	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
		Player *m_player = iter->second;
		if (m_player != nullptr) {
			if (m_player->getStats()->getLevel() < lowbound || m_player->getStats()->getLevel() > highbound) {
				is = false;
				break;
			}
		}
	}
	return is;
}

void Party::warpAllMembers(int32_t mapid, const string &portalname) {
	if (Maps::getMap(mapid)) {
		PortalInfo *portal = nullptr;
		if (portalname != "") { // Optional portal parameter
			portal = Maps::getMap(mapid)->getPortal(portalname);
		}
		for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
			if (Player *m_player = iter->second) {
				m_player->setMap(mapid, portal);
			}
		}
	}
}

void Party::sendPacket(PacketCreator &packet, int32_t mapid) {
	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
		Player *m_player = iter->second;
		if (m_player != nullptr && (mapid != -1 ? m_player->getMap() == mapid : true)) {
			m_player->getSession()->send(packet);
		}
	}
}

bool Party::checkFootholds(int8_t membercount, const vector<vector<int16_t> > &footholds) {
	// Determines if the players are properly arranged (i.e. 5 people on 5 barrels in Kerning PQ)
	bool winner = true;
	int8_t membersonfootholds = 0;
	unordered_map<size_t, bool> footholdhasplayer; // foothold group ID = key
	for (size_t m = 0; m < footholds.size(); m++) {
		footholdhasplayer[m] = false;
		for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
			if (Player *m_player = iter->second) {
				for (size_t k = 0; k < footholds[m].size(); k++) {
					if (m_player->getFh() == footholds[m][k]) {
						if (footholdhasplayer[m]) {
							winner = false;
						}
						else {
							footholdhasplayer[m] = true;
							membersonfootholds++;
						}
						break;
					}
				}
			}
			if (!winner)
				break;
		}
		if (!winner)
			break;
	}
	if (winner && (membersonfootholds != membercount)) // Not all the foothold groups were indexed
		winner = false;
	return winner;
}

bool Party::verifyFootholds(const vector<vector<int16_t> > &footholds) {
	// Determines if the players match your selected footholds
	bool winner = true;
	unordered_map<size_t, bool> footholdhasplayer; // foothold group ID = key
	for (size_t m = 0; m < footholds.size(); m++) {
		footholdhasplayer[m] = false;
		for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
			if (Player *m_player = iter->second) {
				for (size_t k = 0; k < footholds[m].size(); k++) {
					if (m_player->getFh() == footholds[m][k]) {
						if (footholdhasplayer[m]) {
							winner = false;
						}
						else {
							footholdhasplayer[m] = true;
						}
						break;
					}
				}
				if (!winner)
					break;
			}
		}
		if (!winner)
			break;
	}
	if (winner) {
		for (unordered_map<size_t, bool>::iterator iter = footholdhasplayer.begin(); iter != footholdhasplayer.end(); iter++) {
			if (!iter->second) {
				winner = false;
				break;
			}
		}
	}
	return winner;
}