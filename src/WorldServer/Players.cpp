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
#include "Players.h"
#include "ChannelChangeRequests.h"
#include "Channels.h"
#include "LoginServerConnectPacket.h"
#include "PartyHandler.h"
#include "StringUtilities.h"
#include "WorldServer.h"

Players * Players::singleton = 0;

void Players::registerPlayer(uint32_t ip, int32_t id, const string &name, uint16_t channel, int32_t map, int32_t job, int32_t level) {
	if (players.find(id) == players.end()) {
		Player *player = new Player();
		player->ip = ip;
		player->id = id;
		player->name = name;
		player->channel = channel;
		player->party = 0;
		player->map = map;
		player->job = job;
		player->level = level;
		player->online = true;
		players[id] = player;

		LoginServerConnectPacket::updateChannelPop(WorldServer::Instance()->getLoginConnection(), channel, ++Channels::Instance()->getChannel(channel)->players);
	}
	else {
		players[id]->channel = channel;
		players[id]->map = map;
		players[id]->job = job;
		players[id]->level = level;
		players[id]->online = true;
		if (players[id]->party != 0) {
			PartyHandler::logInLogOut(id);
		}
	}
}

void Players::remove(int32_t id, int16_t channel) {
	if (channel == -1 || players[id]->channel == channel) {
		players[id]->online = false;
		if (players[id]->party != 0) {
			PartyHandler::logInLogOut(id);
		}
		LoginServerConnectPacket::updateChannelPop(WorldServer::Instance()->getLoginConnection(), channel, --Channels::Instance()->getChannel(channel)->players);
	}
}

Player * Players::getPlayerFromName(const string &name, bool includeOffline) {
	Player *player;
	bool found = false;
	for (unordered_map<int32_t, Player *>::iterator iter = players.begin(); iter != players.end(); iter++) {
		if ((iter->second->online || includeOffline) && StringUtilities::noCaseCompare(iter->second->name, name) == 0) {
			player = iter->second;
			found = true;
			break;
		}
	}
	if (!found) {
		player = new Player;
		player->channel = -1;
	}
	return player;
}

int32_t Players::size() {
	return players.size();
}

Player * Players::getPlayer(int32_t id, bool includeOffline) {
	if (players.find(id) != players.end()) {
		Player *player = players[id];
		if (player->online || includeOffline)
			return player;
	}
	return 0;
}

void Players::removeChannelPlayers(uint16_t channel) {
	for (unordered_map<int32_t, Player *>::iterator iter = players.begin(); iter != players.end(); iter++) {
		if (iter->second->channel == channel) {
			iter->second->online = false;
			ChannelChangeRequests::Instance()->removePendingPlayerEarly(iter->second->id);
		}
	}
}