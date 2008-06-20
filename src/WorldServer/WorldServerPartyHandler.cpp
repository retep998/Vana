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

#include "WorldServerPartyHandler.h"
#include "WorldServerAcceptPlayer.h"
#include "WorldServerPartyPacket.h"
#include "Channels.h"

hash_map <int, Party*> WorldServerPartyHandler::parties;
int WorldServerPartyHandler::partyCount = 0x01;

void WorldServerPartyHandler::silentUpdate(int playerid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	Party *party = parties[pplayer->party];
	for (hash_map<int, Player*>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			WorldServerPartyPacket::updateParty(channel, PARTY_SILENT_UPDATE, iter->second->id);
		}
	}
}

void WorldServerPartyHandler::logInLogOut(int playerid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	Party *party = parties[pplayer->party];
	for (hash_map<int, Player*>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			WorldServerPartyPacket::updateParty(channel, PARTY_LOG_IN_OUT, iter->second->id);
		}
	}
}

void WorldServerPartyHandler::createParty(WorldServerAcceptPlayer *player, int playerid) {
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
	WorldServerPartyPacket::createParty(player, playerid);
}

void WorldServerPartyHandler::giveLeader(WorldServerAcceptPlayer *player, int playerid, int target, bool is) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party == 0 || !parties[pplayer->party]->isLeader(playerid)) {
		return; //Hacking
	}
	Party *party = parties[pplayer->party];
	party->setLeader(target);
	for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			WorldServerPartyPacket::giveLeader(channel, iter->second->id, target, is);
		}
	}
}

void WorldServerPartyHandler::expelPlayer(WorldServerAcceptPlayer *player, int playerid, int target) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party == 0 || !parties[pplayer->party]->isLeader(playerid)) {
		return; //Hacking
	}
	Party *party = parties[pplayer->party];
	party->deleteMember(Players::Instance()->getPlayer(target)); 
	for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
		if (iter->second->online) {
			WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
			WorldServerPartyPacket::updateParty(channel, PARTY_EXPEL, playerid, target);
		}
	}
	if (Players::Instance()->getPlayer(target)->online) {
		WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(target)->player;
		WorldServerPartyPacket::updateParty(channel, PARTY_EXPEL, target, target);
	}
	Players::Instance()->getPlayer(target)->party = 0;
}

void WorldServerPartyHandler::leaveParty(WorldServerAcceptPlayer *player, int playerid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party == 0) {
		return; //hacking
	}
	Party *party = parties[pplayer->party];
	if (party->isLeader(playerid)) {
		for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->online) {
				WorldServerPartyPacket::disbandParty(Channels::Instance()->getChannel(iter->second->channel)->player, iter->second->id);
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
				WorldServerPartyPacket::updateParty(channel, PARTY_LEAVE, iter->second->id, playerid);
			}
		}
		WorldServerPartyPacket::updateParty(Channels::Instance()->getChannel(pplayer->channel)->player, PARTY_LEAVE, playerid, playerid);
		pplayer->party = 0;
	}
}

void WorldServerPartyHandler::joinParty(WorldServerAcceptPlayer *player, int playerid, int partyid) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party != 0) {
		return; //Hacking
	}
	Party *party = parties[partyid];
	if (party->members.size() == 6) {
		WorldServerPartyPacket::partyError(player, playerid, 0x11);
	}
	else {
		pplayer->party = party->getId();
		party->addMember(pplayer);
		for (hash_map<int, Player *>::iterator iter = party->members.begin(); iter != party->members.end(); iter++) {
			if (iter->second->online) {
				WorldServerAcceptPlayer *channel = Channels::Instance()->getChannel(iter->second->channel)->player;
				WorldServerPartyPacket::updateParty(channel, PARTY_JOIN, iter->second->id, playerid);
			}
		}
	}
}

void WorldServerPartyHandler::invitePlayer(WorldServerAcceptPlayer *player, int playerid, const string &invitee) {
	Player *pplayer = Players::Instance()->getPlayer(playerid);
	if (pplayer->party == 0 || !parties[pplayer->party]->isLeader(playerid)) {
		return; //hacking
	}
	Player *invited = Players::Instance()->getPlayerFromName(invitee);
	if (invited->channel != -1 && invited->channel == pplayer->channel) {
		if (invited->party != 0) {
			WorldServerPartyPacket::partyError(player, playerid, 0x10);
		}
		else {
			WorldServerPartyPacket::invitePlayer(player, invited->id, pplayer->name);
		}
	}
	else {
		WorldServerPartyPacket::partyError(player, playerid, 0x12);
	}
}
