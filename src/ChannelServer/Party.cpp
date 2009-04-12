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
#include "Party.h"
#include "ChannelServer.h"
#include "Instance.h"
#include "InstanceMessageConstants.h"
#include "Maps.h"
#include "PacketReader.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "WorldServerConnectPacket.h"

unordered_map<int32_t, Party *> PartyFunctions::parties;

void PartyFunctions::handleRequest(Player *player, PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	switch (type) {
		case 0x01: // Create party
		case 0x02: // Leave party
			WorldServerConnectPacket::partyOperation(ChannelServer::Instance()->getWorldPlayer(), type, player->getId());
			break;
		case 0x03: // Join party
		case 0x05: // Expel Player
		case 0x06: // Give leader rights
			WorldServerConnectPacket::partyOperation(ChannelServer::Instance()->getWorldPlayer(), type, player->getId(), packet.get<int32_t>());
			break;
		case 0x04: // Invite
			WorldServerConnectPacket::partyInvite(ChannelServer::Instance()->getWorldPlayer(), player->getId(), packet.getString());
			break;
	}
}

void PartyFunctions::handleResponse(PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	int32_t playerid = packet.get<int32_t>();
	int32_t partyid = packet.get<int32_t>();
	Player *player = Players::Instance()->getPlayer(playerid);
	Party *party = parties.find(partyid) != parties.end() ? parties[partyid] : 0;
	if (player == 0 || party == 0)
		return;
	switch (type) {
		case 0x01: // Leave / Disband
		case 0x06: // Expel
			party->deleteMember(player);
			break;
		case 0x02: // Create / Join
			player->setParty(party);
			party->addMember(player);
			party->showHPBar(player);
			party->receiveHPBar(player);
			break;
		case 0x05: // LogInLogOut
			if (player->getParty() == 0) {
				player->setParty(party);
				party->setMember(player->getId(), player);
				party->showHPBar(player);
				party->receiveHPBar(player);
			}
			break;
	}
}

void PartyFunctions::handleDataSync(PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	switch (type) {
		case PARTY_SYNC_CHANNEL_START:
			handleChannelStart(packet);
			break;
		case PARTY_SYNC_DISBAND:
			disbandParty(packet);
			break;
		case PARTY_SYNC_CREATE: {
			int32_t partyid = packet.get<int32_t>();
			int32_t leaderid = packet.get<int32_t>();
			Party *party = new Party(partyid);
			Player *leader = Players::Instance()->getPlayer(leaderid);
			if (leader == 0) {
				party->addMember(leaderid);
			}
			else {
				party->addMember(leader);
			}
			party->setLeader(leaderid, true);
			parties[partyid] = party;
			break;
		}
		case PARTY_SYNC_SWITCH_LEADER: {
			int32_t partyid = packet.get<int32_t>();
			if (parties.find(partyid) != parties.end()) {
				Party *party = parties[partyid];
				int32_t newleader = packet.get<int32_t>();
				party->setLeader(newleader);
			}
			break;
		}
		case PARTY_SYNC_REMOVE_MEMBER: {
			int32_t partyid = packet.get<int32_t>();
			if (parties.find(partyid) != parties.end()) {
				Party *party = parties[partyid];
				int32_t playerid = packet.get<int32_t>();
				Player *member = Players::Instance()->getPlayer(playerid);
				if (member == 0) {
					party->deleteMember(playerid);
				}
				else {
					party->deleteMember(member);
				}
			}
			break;
		}
		case PARTY_SYNC_ADD_MEMBER: {
			int32_t partyid = packet.get<int32_t>();
			if (parties.find(partyid) != parties.end()) {
				Party *party = parties[partyid];
				int32_t playerid = packet.get<int32_t>();
				Player *member = Players::Instance()->getPlayer(playerid);
				if (member == 0) {
					party->addMember(playerid);
				}
				else {
					party->addMember(member);
				}
			}
			break;
		}
	}
}

void PartyFunctions::handleChannelStart(PacketReader &packet) {
	int32_t numberparties = packet.get<int32_t>();
	for (int32_t i = 0; i < numberparties; i++) {
		int32_t partyid = packet.get<int32_t>();
		int8_t membersnum = packet.get<int8_t>();
		Party *party = new Party(partyid);
		for (int8_t j = 0; j < membersnum; j++) {
			int32_t memberid = packet.get<int32_t>();
			party->addMember(memberid);
		}
		int32_t leaderid = packet.get<int32_t>();
		party->setLeader(leaderid, true);
		parties[partyid] = party;
	}
}

void PartyFunctions::disbandParty(PacketReader &packet) {
	int32_t partyid = packet.get<int32_t>();
	if (parties.find(partyid) != parties.end()) {
		Party *party = parties[partyid];
		party->disband();
		delete party;
		parties.erase(partyid);
	}
}

// Party class
void Party::setLeader(int32_t playerid, bool firstload) {
	if (!firstload) {
		oldleader.push_back(members[leaderid]->getId());
	}
	leaderid = playerid;
}

void Party::addMember(Player *player) {
	members[player->getId()] = player;
	player->setParty(this);
}

void Party::addMember(int32_t id) {
	members[id] = 0;
}

void Party::deleteMember(Player *player) {
	members.erase(player->getId());
	player->setParty(0);
	if (getInstance() != 0) {
		getInstance()->sendMessage(Party_Remove_Member, getId(), player->getId());
	}
}

void Party::deleteMember(int32_t id) {
	if (getInstance() != 0) {
		getInstance()->sendMessage(Party_Remove_Member, getId(), id);
	}
	members.erase(id);
}

void Party::disband() {
	if (getInstance() != 0) {
		getInstance()->sendMessage(Party_Disband, getId());
		setInstance(0);
	}
	map<int32_t, Player *, std::greater<int32_t> > temp = members;
	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = temp.begin(); iter != temp.end(); iter++) {
		if (iter->second != 0) {
			iter->second->setParty(0);
			members.erase(iter->first);
		}
	}
}

Player * Party::getMemberByIndex(uint8_t index) { 
	Player *p = 0;
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

void Party::setMember(int32_t playerid, Player *player) {
	members[playerid] = player;
}

void Party::showHPBar(Player *player) {
	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
		Player *m_player = iter->second;
		if (m_player != 0 && m_player != player && m_player->getMap() == player->getMap())
			PlayerPacket::showHPBar(player, m_player);
	}
}

void Party::receiveHPBar(Player *player) {
	for (map<int32_t, Player *, std::greater<int32_t> >::iterator iter = members.begin(); iter != members.end(); iter++) {
		Player *m_player = iter->second;
		if (m_player != 0 && m_player != player && m_player->getMap() == player->getMap())
			PlayerPacket::showHPBar(m_player, player);
	}	
}