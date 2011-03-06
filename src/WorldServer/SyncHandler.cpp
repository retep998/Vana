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
#include "SyncHandler.h"
#include "Channel.h"
#include "Channels.h"
#include "Database.h"
#include "GameObjects.h"
#include "InterHeader.h"
#include "InterHelper.h"
#include "IpUtilities.h"
#include "MapleSession.h"
#include "PacketCreator.h"
#include "PacketReader.h"
#include "Party.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "SendHeader.h"
#include "SyncPacket.h"
#include "WorldServer.h"
#include "WorldServerAcceptConnection.h"
#include "WorldServerAcceptPacket.h"

void SyncHandler::handle(WorldServerAcceptConnection *connection, PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::SyncTypes::Player: handlePlayerPacket(connection, packet); break;
		case Sync::SyncTypes::Party: partyOperation(packet); break;
	}
}

void SyncHandler::silentUpdate(int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	Party *party = pplayer->getParty();
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {
			SyncPacket::PartyPacket::updateParty(iter->second->getChannel(), PartyActions::SilentUpdate, iter->second->getId());
		}
	}
}

void SyncHandler::logInLogOut(int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid, true);
	Party *party = pplayer->getParty();
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {
			SyncPacket::PartyPacket::updateParty(iter->second->getChannel(), PartyActions::LogInOrOut, iter->second->getId());
		}
	}
}

void SyncHandler::createParty(int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() != nullptr) {
		// Hacking
		return;
	}
	Party *party = new Party(PlayerDataProvider::Instance()->getPartyId());
	party->addMember(pplayer);
	party->setLeader(pplayer->getId());
	PlayerDataProvider::Instance()->addParty(party);
	pplayer->setParty(party);

	SyncPacket::PartyPacket::createParty(pplayer->getChannel(), playerid);
	SyncPacket::PlayerPacket::sendCreateParty(pplayer->getId(), pplayer->getParty()->getId());
}

void SyncHandler::giveLeader(int32_t playerid, int32_t target, bool is) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == nullptr || !pplayer->getParty()->isLeader(playerid)) {
		// Hacking
		return;
	}
	Party *party = pplayer->getParty();
	party->setLeader(target);
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); ++iter) {
		if (iter->second->isOnline()) {
			SyncPacket::PartyPacket::giveLeader(iter->second->getChannel(), iter->second->getId(), target, is);
		}
	}
	SyncPacket::PlayerPacket::sendSwitchPartyLeader(target, party->getId());
}

void SyncHandler::expelPlayer(int32_t playerid, int32_t target) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	Player *tplayer = PlayerDataProvider::Instance()->getPlayer(target);
	if (pplayer->getParty() == nullptr || !pplayer->getParty()->isLeader(playerid)) {
		// Hacking
		return;
	}
	Party *party = pplayer->getParty();
	party->deleteMember(target);
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); ++iter) {
		if (iter->second->isOnline()) {
			SyncPacket::PartyPacket::updateParty(iter->second->getChannel(), PartyActions::Expel, iter->first, target);
		}
	}
	if (tplayer != nullptr) {
		SyncPacket::PartyPacket::updateParty(tplayer->getChannel(), PartyActions::Expel, target, target);
	}
	SyncPacket::PlayerPacket::sendRemovePartyPlayer(pplayer->getId(), party->getId());
	PlayerDataProvider::Instance()->getPlayer(target, true)->setParty(nullptr);
}

void SyncHandler::leaveParty(int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == nullptr) {
		// Hacking
		return;
	}
	Party *party = pplayer->getParty();
	if (party->isLeader(playerid)) {
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->isOnline()) {
				SyncPacket::PartyPacket::disbandParty(iter->second->getChannel(), iter->second->getId());
				iter->second->setParty(nullptr);
			}
		}
		SyncPacket::PlayerPacket::sendDisbandParty(party->getId());
		PlayerDataProvider::Instance()->removeParty(party->getId());
	}
	else {
		SyncPacket::PlayerPacket::sendRemovePartyPlayer(pplayer->getId(), pplayer->getParty()->getId());
		party->deleteMember(pplayer->getId());
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->isOnline()) {
				SyncPacket::PartyPacket::updateParty(iter->second->getChannel(), PartyActions::Leave, iter->second->getId(), playerid);
			}
		}
		SyncPacket::PartyPacket::updateParty(pplayer->getChannel(), PartyActions::Leave, playerid, playerid);
		pplayer->setParty(nullptr);
	}
}

void SyncHandler::joinParty(int32_t playerid, int32_t partyid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() != nullptr) {
		// Hacking
		return;
	}
	Party *party = PlayerDataProvider::Instance()->getParty(partyid);
	if (party->members.size() == 6) {
		SyncPacket::PartyPacket::partyError(pplayer->getChannel(), playerid, 0x11);
	}
	else {
		pplayer->setParty(party);
		party->addMember(pplayer);
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); ++iter) {
			if (iter->second->isOnline()) {
				SyncPacket::PartyPacket::updateParty(iter->second->getChannel(), PartyActions::Join, iter->second->getId(), playerid);
			}
		}
		SyncPacket::PlayerPacket::sendAddPartyPlayer(playerid, partyid);
	}
}

void SyncHandler::invitePlayer(int32_t playerid, const string &invitee) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == nullptr || !pplayer->getParty()->isLeader(playerid)) {
		// Hacking
		return;
	}
	Player *invited = PlayerDataProvider::Instance()->getPlayer(invitee);
	if (invited->isOnline() && invited->getChannel() == pplayer->getChannel()) {
		if (invited->getParty() != nullptr) {
			SyncPacket::PartyPacket::partyError(pplayer->getChannel(), playerid, 0x10);
		}
		else {
			SyncPacket::PartyPacket::invitePlayer(pplayer->getChannel(), invited->getId(), pplayer->getName());
		}
	}
	else {
		SyncPacket::PartyPacket::partyError(pplayer->getChannel(), playerid, 0x12);
	}
}

void SyncHandler::handlePlayerPacket(WorldServerAcceptConnection *connection, PacketReader &packet) {
	switch (packet.get<int8_t>()) {
		case Sync::Player::ChangeChannelRequest: playerChangeChannel(connection, packet); break;
		case Sync::Player::ChangeChannelGo: handleChangeChannel(connection, packet); break;
		case Sync::Player::Connect: playerConnect(connection->getChannel(), packet); break;
		case Sync::Player::Disconnect: playerDisconnect(connection->getChannel(), packet); break;
		case Sync::Player::UpdateLevel: updateLevel(packet); break;
		case Sync::Player::UpdateJob: updateJob(packet); break;
		case Sync::Player::UpdateMap: updateMap(packet); break;
	}
}

void SyncHandler::playerConnect(uint16_t channel, PacketReader &packet) {
	uint32_t ip = packet.get<uint32_t>();
	int32_t id = packet.get<int32_t>();
	string name = packet.getString();
	int32_t map = packet.get<int32_t>();
	int16_t job = static_cast<int16_t>(packet.get<int32_t>());
	uint8_t level = static_cast<uint8_t>(packet.get<int32_t>());

	Player *p = PlayerDataProvider::Instance()->getPlayer(id, true);
	if (p == nullptr) {
		p = new Player(id);
	}
	p->setIp(ip);
	p->setName(name);
	p->setMap(map);
	p->setJob(job);
	p->setLevel(level);
	p->setChannel(channel);
	p->setOnline(true);
	PlayerDataProvider::Instance()->registerPlayer(p);
}

void SyncHandler::playerDisconnect(uint16_t channel, PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	PlayerDataProvider::Instance()->remove(id, channel);
	int16_t chan = PlayerDataProvider::Instance()->removePendingPlayerEarly(id);
	if (chan != -1) {
		SyncPacket::PlayerPacket::sendHeldPacketRemoval(chan, id);
	}
}
void SyncHandler::partyOperation(PacketReader &packet) {
	int8_t type = packet.get<int8_t>();
	int32_t playerid = packet.get<int32_t>();
	switch (type) {
		case PartyActions::Create: createParty(playerid); break;
		case PartyActions::Leave: leaveParty(playerid); break;
		case PartyActions::Join: joinParty(playerid, packet.get<int32_t>()); break;
		case PartyActions::Invite: invitePlayer(playerid, packet.getString()); break;
		case PartyActions::Expel: expelPlayer(playerid, packet.get<int32_t>()); break;
		case PartyActions::SetLeader: giveLeader(playerid, packet.get<int32_t>(), false); break;
	}
}

void SyncHandler::playerChangeChannel(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Channel *chan = Channels::Instance()->getChannel(packet.get<int16_t>());
	if (chan) {
		SyncPacket::PlayerPacket::sendPacketToChannelForHolding(chan->getId(), playerid, packet);
		PlayerDataProvider::Instance()->addPendingPlayer(playerid, chan->getId());
	}
	else { // Channel doesn't exist (offline)
		SyncPacket::PlayerPacket::playerChangeChannel(player, playerid, 0, -1);
	}
}

void SyncHandler::handleChangeChannel(WorldServerAcceptConnection *player, PacketReader &packet) {
	int32_t playerid = packet.get<int32_t>();
	Player *gamePlayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (gamePlayer) {
		uint16_t chanid = PlayerDataProvider::Instance()->getPendingPlayerChannel(playerid);
		Channel *chan = Channels::Instance()->getChannel(chanid);
		Channel *curchan = Channels::Instance()->getChannel(gamePlayer->getChannel());
		if (chan) {
			SyncPacket::PlayerPacket::newConnectable(chan->getId(), playerid, gamePlayer->getIp());
			uint32_t chanIp = IpUtilities::matchIpSubnet(gamePlayer->getIp(), chan->getExternalIps(), chan->getIp());
			SyncPacket::PlayerPacket::playerChangeChannel(curchan->getConnection(), playerid, chanIp, chan->getPort());
		}
		else {
			SyncPacket::PlayerPacket::playerChangeChannel(curchan->getConnection(), playerid, 0, -1);
		}
		PlayerDataProvider::Instance()->removePendingPlayer(playerid);
	}
}

void SyncHandler::updateJob(PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int16_t job = packet.get<int16_t>();
	Player *plyr = PlayerDataProvider::Instance()->getPlayer(id);
	plyr->setJob(job);
	if (plyr->getParty() != nullptr) {
		SyncHandler::silentUpdate(id);
	}
}

void SyncHandler::updateLevel(PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	uint8_t level = packet.get<uint8_t>();
	Player *plyr = PlayerDataProvider::Instance()->getPlayer(id);
	plyr->setLevel(level);
	if (plyr->getParty() != nullptr) {
		SyncHandler::silentUpdate(id);
	}
}

void SyncHandler::updateMap(PacketReader &packet) {
	int32_t id = packet.get<int32_t>();
	int32_t map = packet.get<int32_t>();
	if (Player *p = PlayerDataProvider::Instance()->getPlayer(id)) {
		p->setMap(map);
		if (p->getParty() != nullptr) {
			SyncHandler::silentUpdate(id);
		}
	}
}
