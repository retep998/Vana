/*
Copyright (C) 2008 Vana Development Team

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
#include "WorldServerAcceptPlayer.h"
#include "PartyPacket.h"
#include "Channels.h"

hash_map <int, Party*> PartyHandler::parties;
int PartyHandler::partyCount = 0x01;

void PartyHandler::silentUpdate(int playerid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	Party *party = parties[pplayer->party];
	for (hash_map<int, Player*>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			PartyPacket::updateParty(channel, PARTY_SILENT_UPDATE, iter->second->id);
		}
	}
}

void PartyHandler::logInLogOut(int playerid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	Party *party = parties[pplayer->party];
	for (hash_map<int, Player*>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			PartyPacket::updateParty(channel, PARTY_LOG_IN_OUT, iter->second->id);
		}
	}
}

void PartyHandler::createParty(WorldServerAcceptPlayer *player, int playerid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party != 0) {
		return; //hacking
	}
	Party *party = new Party;
	party->setId(partyCount);
	party->addMember(pplayer);
	party->setLeader(pplayer->id);
	parties[partyCount] = party;
	pplayer->party = partyCount;
	partyCount++;
	PartyPacket::createParty(player, playerid);
}

void PartyHandler::giveLeader(WorldServerAcceptPlayer *player, int playerid, int target, bool is) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party == 0 || !parties[pplayer->party]->isLeader(playerid)) {
		return; //Hacking
	}
	Party *party = parties[pplayer->party];
	party->setLeader(target);
	for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			PartyPacket::giveLeader(channel, iter->second->id, target, is);
		}
	}
}

void PartyHandler::expelPlayer(WorldServerAcceptPlayer *player, int playerid, int target) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	Player *tplayer = Players::Instance()->getPlayer(target);
	if (pplayer->party == 0 || !parties[pplayer->party]->isLeader(playerid)) {
		return; //Hacking
	}
	Party *party = parties[pplayer->party];
	party->deleteMember(tplayer); 
	for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			PartyPacket::updateParty(channel, PARTY_EXPEL, playerid, target);
		}
	}
	if (tplayer->online) {
		WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(tplayer->channel)->player;
		PartyPacket::updateParty(channel, PARTY_EXPEL, target, target);
	}
	Players::Instance()->getPlayer(target)->party = 0;
}

void PartyHandler::leaveParty(WorldServerAcceptPlayer *player, int playerid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party == 0) {
		return; //hacking
	}
	Party *party = parties[pplayer->party];
	if (party->isLeader(playerid)) {
		for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->online) {
				PartyPacket::disbandParty(Channels::Instance()->getChannel(iter->second->channel)->player, iter->second->id);
				iter->second->party = 0;
			}
		}
		parties.erase(pplayer->party);
	}
	else {
		party->deleteMember(pplayer);
		for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->online) {			
				WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
				PartyPacket::updateParty(channel, PARTY_LEAVE, iter->second->id, playerid);
			}
		}
		PartyPacket::updateParty(Channels::Instance()->getChannel(pplayer->channel)->player, PARTY_LEAVE, playerid, playerid);
		pplayer->party = 0;
	}
}

void PartyHandler::joinParty(WorldServerAcceptPlayer *player, int playerid, int partyid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party != 0) {
		return; //Hacking
	}
	Party *party = parties[partyid];
	if (party->members.size() == 6) {
		PartyPacket::partyError(player, playerid, 0x11);
	}
	else {
		pplayer->party = party->getId();
		party->addMember(pplayer);
		for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->online) {
				WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
				PartyPacket::updateParty(channel, PARTY_JOIN, iter->second->id, playerid);
			}
		}
	}
}

void PartyHandler::invitePlayer(WorldServerAcceptPlayer *player, int playerid, const string &invitee) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party == 0 || !parties[pplayer->party]->isLeader(playerid)) {
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
