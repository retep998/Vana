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
#include "PartyHandler.h"
#include "Channel.h"
#include "Channels.h"
#include "InterHelper.h"
#include "Party.h"
#include "PartyPacket.h"
#include "Player.h"
#include "PlayerDataProvider.h"
#include "WorldServerAcceptConnection.h"
#include "WorldServerAcceptPacket.h"

void PartyHandler::silentUpdate(int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	Party *party = pplayer->getParty();
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {
			PartyPacket::updateParty(iter->second->getChannel(), PartyActions::SilentUpdate, iter->second->getId());
		}
	}
}

void PartyHandler::logInLogOut(int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid, true);
	Party *party = pplayer->getParty();
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {
			PartyPacket::updateParty(iter->second->getChannel(), PartyActions::LogInOrOut, iter->second->getId());
		}
	}
}

void PartyHandler::createParty(WorldServerAcceptConnection *player, int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() != 0) {
		// Hacking
		return;
	}
	Party *party = new Party(PlayerDataProvider::Instance()->getPartyId());
	party->addMember(pplayer);
	party->setLeader(pplayer->getId());
	PlayerDataProvider::Instance()->addParty(party);
	pplayer->setParty(party);

	PartyPacket::createParty(pplayer->getChannel(), playerid);

	WorldServerAcceptPacket::sendCreateParty(pplayer->getId(), pplayer->getParty()->getId());
}

void PartyHandler::giveLeader(WorldServerAcceptConnection *player, int32_t playerid, int32_t target, bool is) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == 0 || !pplayer->getParty()->isLeader(playerid)) {
		// Hacking
		return;
	}
	Party *party = pplayer->getParty();
	party->setLeader(target);
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {
			PartyPacket::giveLeader(iter->second->getChannel(), iter->second->getId(), target, is);
		}
	}
	WorldServerAcceptPacket::sendSwitchPartyLeader(target, pplayer->getParty()->getId());
}

void PartyHandler::expelPlayer(WorldServerAcceptConnection *player, int32_t playerid, int32_t target) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	Player *tplayer = PlayerDataProvider::Instance()->getPlayer(target);
	if (pplayer->getParty() == 0 || !pplayer->getParty()->isLeader(playerid)) {
		// Hacking
		return;
	}
	Party *party = pplayer->getParty();
	party->deleteMember(target);
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->isOnline()) {
			PartyPacket::updateParty(iter->second->getChannel(), PartyActions::Expel, iter->first, target);
		}
	}
	if (tplayer != 0) {
		PartyPacket::updateParty(tplayer->getChannel(), PartyActions::Expel, target, target);
	}
	WorldServerAcceptPacket::sendRemovePartyPlayer(target, pplayer->getParty()->getId());
	PlayerDataProvider::Instance()->getPlayer(target, true)->setParty(0);
}

void PartyHandler::leaveParty(WorldServerAcceptConnection *player, int32_t playerid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == 0) {
		// Hacking
		return;
	}
	Party *party = pplayer->getParty();
	if (party->isLeader(playerid)) {
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->isOnline()) {
				PartyPacket::disbandParty(iter->second->getChannel(), iter->second->getId());
				iter->second->setParty(0);
			}
		}
		WorldServerAcceptPacket::sendDisbandParty(party->getId());
		PlayerDataProvider::Instance()->removeParty(party->getId());
	}
	else {
		WorldServerAcceptPacket::sendRemovePartyPlayer(pplayer->getId(), pplayer->getParty()->getId());
		party->deleteMember(pplayer->getId());
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->isOnline()) {
				PartyPacket::updateParty(iter->second->getChannel(), PartyActions::Leave, iter->second->getId(), playerid);
			}
		}
		PartyPacket::updateParty(pplayer->getChannel(), PartyActions::Leave, playerid, playerid);
		pplayer->setParty(0);
	}
}

void PartyHandler::joinParty(WorldServerAcceptConnection *player, int32_t playerid, int32_t partyid) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() != 0) {
		// Hacking
		return;
	}
	Party *party = PlayerDataProvider::Instance()->getParty(partyid);
	if (party->members.size() == 6) {
		PartyPacket::partyError(pplayer->getChannel(), playerid, 0x11);
	}
	else {
		pplayer->setParty(party);
		party->addMember(pplayer);
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->isOnline()) {
				PartyPacket::updateParty(iter->second->getChannel(), PartyActions::Join, iter->second->getId(), playerid);
			}
		}
		WorldServerAcceptPacket::sendAddPartyPlayer(playerid, partyid);
	}
}

void PartyHandler::invitePlayer(WorldServerAcceptConnection *player, int32_t playerid, const string &invitee) {
	Player *pplayer = PlayerDataProvider::Instance()->getPlayer(playerid);
	if (pplayer->getParty() == 0 || !pplayer->getParty()->isLeader(playerid)) {
		// Hacking
		return;
	}
	Player *invited = PlayerDataProvider::Instance()->getPlayer(invitee);
	if (invited->isOnline() && invited->getChannel() == pplayer->getChannel()) {
		if (invited->getParty() != 0) {
			PartyPacket::partyError(pplayer->getChannel(), playerid, 0x10);
		}
		else {
			PartyPacket::invitePlayer(pplayer->getChannel(), invited->getId(), pplayer->getName());
		}
	}
	else {
		PartyPacket::partyError(pplayer->getChannel(), playerid, 0x12);
	}
}
