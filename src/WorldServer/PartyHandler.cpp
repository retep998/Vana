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
#include "Channels.h"
#include "Parties.h"
#include "PartyPacket.h"
#include "WorldServerAcceptConnection.h"
#include "WorldServerAcceptPacket.h"

void PartyHandler::silentUpdate(int32_t playerid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	Party *party = Parties::Instance()->getParty(pplayer->party);
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptConnection *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			PartyPacket::updateParty(channel, PARTY_SILENT_UPDATE, iter->second->id);
		}
	}
}

void PartyHandler::logInLogOut(int32_t playerid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid, true);
	Party *party = Parties::Instance()->getParty(pplayer->party);
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptConnection *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			PartyPacket::updateParty(channel, PARTY_LOG_IN_OUT, iter->second->id);
		}
	}
}

void PartyHandler::createParty(WorldServerAcceptConnection *player, int32_t playerid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party != 0) {
		return; // hacking
	}
	Party *party = new Party;
	party->addMember(pplayer);
	party->setLeader(pplayer->id);
	pplayer->party = Parties::Instance()->addParty(party);
	PartyPacket::createParty(player, playerid);
	WorldServerAcceptPacket::sendCreateParty(pplayer->id, pplayer->party);
}

void PartyHandler::giveLeader(WorldServerAcceptConnection *player, int32_t playerid, int32_t target, bool is) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party == 0 || !Parties::Instance()->getParty(pplayer->party)->isLeader(playerid)) {
		return; //Hacking
	}
	Party *party = Parties::Instance()->getParty(pplayer->party);
	party->setLeader(target);
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptConnection *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			PartyPacket::giveLeader(channel, iter->second->id, target, is);
		}
	}
	WorldServerAcceptPacket::sendSwitchPartyLeader(target, pplayer->party);
}

void PartyHandler::expelPlayer(WorldServerAcceptConnection *player, int32_t playerid, int32_t target) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	Player *tplayer = Players::Instance()->getPlayer(target);
	if (pplayer->party == 0 || !Parties::Instance()->getParty(pplayer->party)->isLeader(playerid)) {
		return; //Hacking
	}
	Party *party = Parties::Instance()->getParty(pplayer->party);
	party->deleteMember(target);
	for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptConnection *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			PartyPacket::updateParty(channel, PARTY_EXPEL, iter->first, target);
		}
	}
	if (tplayer != 0) {
		WorldServerAcceptConnection *channel = Channels::Instance()->getChannel(tplayer->channel)->player;
		PartyPacket::updateParty(channel, PARTY_EXPEL, target, target);
	}
	WorldServerAcceptPacket::sendRemovePartyPlayer(target, pplayer->party);
	Players::Instance()->getPlayer(target, true)->party = 0;
}

void PartyHandler::leaveParty(WorldServerAcceptConnection *player, int32_t playerid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party == 0) {
		return; //hacking
	}
	Party *party = Parties::Instance()->getParty(pplayer->party);
	if (party->isLeader(playerid)) {
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->online) {
				PartyPacket::disbandParty(Channels::Instance()->getChannel(iter->second->channel)->player, iter->second->id);
				iter->second->party = 0;
			}
		}
		WorldServerAcceptPacket::sendDisbandParty(pplayer->party);
		Parties::Instance()->removeParty(pplayer->party);
	}
	else {
		WorldServerAcceptPacket::sendRemovePartyPlayer(pplayer->id, pplayer->party);
		party->deleteMember(pplayer->id);
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->online) {
				WorldServerAcceptConnection *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
				PartyPacket::updateParty(channel, PARTY_LEAVE, iter->second->id, playerid);
			}
		}
		PartyPacket::updateParty(Channels::Instance()->getChannel(pplayer->channel)->player, PARTY_LEAVE, playerid, playerid);
		pplayer->party = 0;
	}
}

void PartyHandler::joinParty(WorldServerAcceptConnection *player, int32_t playerid, int32_t partyid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party != 0) {
		return; //Hacking
	}
	Party *party = Parties::Instance()->getParty(partyid);
	if (party->members.size() == 6) {
		PartyPacket::partyError(player, playerid, 0x11);
	}
	else {
		pplayer->party = partyid;
		party->addMember(pplayer);
		for (map<int32_t, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->online) {
				WorldServerAcceptConnection *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
				PartyPacket::updateParty(channel, PARTY_JOIN, iter->second->id, playerid);
			}
		}
		WorldServerAcceptPacket::sendAddPartyPlayer(playerid, partyid);
	}
}

void PartyHandler::invitePlayer(WorldServerAcceptConnection *player, int32_t playerid, const string &invitee) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party == 0 || !Parties::Instance()->getParty(pplayer->party)->isLeader(playerid)) {
		return; //hacking
	}
	Player *invited = Players::Instance()->getPlayerFromName(invitee);
	if (invited->channel != -1 && invited->channel == pplayer->channel) {
		if (invited->party != 0) {
			PartyPacket::partyError(player, playerid, 0x10);
		}
		else {
			PartyPacket::invitePlayer(player, invited->id, pplayer->name);
		}
	}
	else {
		PartyPacket::partyError(player, playerid, 0x12);
	}
}
